#include "pch.h"
#include "LineDBManager.h"
#include "LoggerManager.h"
#include "FTSDKConfigManager.h"
#include "LineProtocolBuilder.h"
#include "Constants.h"
#include <iostream>
#include <sstream>

#define   LINEDB_TABLE_CREATE_LINE_RUM_SQL      "CREATE TABLE IF NOT EXISTS line_rum(id INTEGER PRIMARY KEY NOT NULL, \
              created_time TimeStamp NOT NULL DEFAULT (datetime('now','localtime')), packet TEXT NOT NULL, \
              status INTEGER NOT NULL default 0, line_cnt INTEGER NOT NULL default 0, retry_cnt INTEGER NOT NULL default 0);"

#define   LINEDB_TABLE_CREATE_LINE_LOG_SQL      "CREATE TABLE IF NOT EXISTS line_log(id INTEGER PRIMARY KEY NOT NULL, \
              created_time TimeStamp NOT NULL DEFAULT (datetime('now','localtime')), packet TEXT NOT NULL, \
              status INTEGER NOT NULL default 0, line_cnt INTEGER NOT NULL default 0, retry_cnt INTEGER NOT NULL default 0);"

#define   LINEDB_TABLE_CREATE_LINE_TRACE_SQL      "CREATE TABLE IF NOT EXISTS line_trace(id INTEGER PRIMARY KEY NOT NULL, \
              created_time TimeStamp NOT NULL DEFAULT (datetime('now','localtime')), packet TEXT NOT NULL, \
              status INTEGER NOT NULL default 0, line_cnt INTEGER NOT NULL default 0, retry_cnt INTEGER NOT NULL default 0);"

namespace com::ft::sdk::internal
{
	LineDBManager::LineDBManager()
	{
        DB_FILE = FTSDKConfigManager::getInstance().getWorkingDir() + "/" + "datakit.db";
	}

	void LineDBManager::init()
	{
        try
        {
            m_logRetentionPolicy = FTSDKConfigManager::getInstance().getLogPipeConfig().getLogCacheDiscardStrategy();
            m_pDB = std::make_shared< SQLite::Database>(DB_FILE, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

            SQLite::Transaction transaction(*m_pDB);

            executeSQL(LINEDB_TABLE_CREATE_LINE_RUM_SQL);
            LoggerManager::getInstance().logInfo("Line DB - RUM is created.");

            executeSQL(LINEDB_TABLE_CREATE_LINE_LOG_SQL);
            LoggerManager::getInstance().logInfo("Line DB - LOG is created.");

            executeSQL(LINEDB_TABLE_CREATE_LINE_TRACE_SQL);
            LoggerManager::getInstance().logInfo("Line DB - TRACE is created.");

            transaction.commit();

            LoggerManager::getInstance().logInfo("Line DB is initialized.");

            if (FTSDKConfigManager::getInstance().getGeneralConfig().getClearDBWhenStartUp())
            {
                clearDB();
            }
        }
        catch (std::exception& e)
        {
            LoggerManager::getInstance().logError("SQLite exception during table creation: {}", e.what());
        }
	}

    void LineDBManager::uninit()
    {

    }

    void LineDBManager::clearDB()
    {
        LoggerManager::getInstance().logDebug("clear Line DB.");

        executeSQL("delete from line_rum");
        executeSQL("delete from line_log");
        executeSQL("delete from line_trace");
    }

    bool LineDBManager::insertLine(DataMsg& msg)
    {
        DataType type = msg.dataType;

        if (type == DataType::RUM_APP)
        {
            return insertRUMLine(msg);
        }
        else if (type == DataType::LOG)
        {
            return insertLogLine(msg);
        }
        else
        {
            return false;
        }
    }

    bool LineDBManager::insertRUMLine(DataMsg& rumMsg)
    {
        bool ret = true;
        DataType type = rumMsg.dataType;

        try
        {
            std::string linePtl = LineProtocolBuilder::getInstance().encode(rumMsg);

            const std::string sql = "insert into " + getTableName(type) + "(line_cnt, packet, status, retry_cnt) values("
                + std::to_string((int)rumMsg.vtLine.size()) + ",'" + linePtl + "'," + std::to_string(LineStatus::ready) + ", 0);";

            int rows = executeSQL(sql);

            if (rows > 0)
            {
                DataSyncManager::getInstance().notifyNewLinefromDB();
            }

            return rows > 0;
        }
        catch (std::exception& e)
        {
            ret = false;
            LoggerManager::getInstance().logError("SQLite exception during insertion: {}", e.what());
        }

        return ret;
    }

    bool LineDBManager::insertLogLine(DataMsg& logMsg)
    {
        bool ret = true;
        DataType type = logMsg.dataType;

        try
        {
            int log2add = logMsg.vtLine.size();
            int log2Remove = reserveRoom4Log(log2add);

            if (log2Remove >= log2add)
            {
                return false;
            }
            while (log2Remove-- > 0)
            {
                logMsg.vtLine.erase(logMsg.vtLine.end() - 1);
            }

            int rows_all = 0;
            for (auto meas : logMsg.vtLine)
            {
                DataMsg msg;
                msg.dataType = type;
                msg.vtLine.push_back(meas);

                std::string linePtl = LineProtocolBuilder::getInstance().encode(msg);

                const std::string sql = "insert into " + getTableName(type) + "(line_cnt, packet, status, retry_cnt) values("
                    + std::to_string((int)msg.vtLine.size()) + ",'" + linePtl + "'," + std::to_string(LineStatus::ready) + ", 0);";

                int rows = executeSQL(sql);
                rows_all += rows;
            }

            if (rows_all > 0)
            {
                DataSyncManager::getInstance().notifyNewLinefromDB();
            }

            return rows_all > 0;
        }
        catch (std::exception& e)
        {
            ret = false;
            LoggerManager::getInstance().logError("SQLite exception during insertion: {}", e.what());
        }

        return ret;
    }

    int LineDBManager::getLineCount(DataType type)
    {
        try
        {
            std::string table = getTableName(type);
            SQLite::Statement  query(*m_pDB, "SELECT SUM(line_cnt) FROM " + table);

            int count = 0;
            while (query.executeStep())
            {
                count = query.getColumn(0);
                break;
            }

            return count;
        }
        catch (std::exception& e)
        {
            LoggerManager::getInstance().logError("exception: {}", e.what());
        }

        return 0;
    }

    /**
     * Query line protocol message from file DB
     * 1. query message by data type
     * 2. delete the message from DB after retrieval
     * 
     * @param type
     * @return 
     */
    std::vector<std::shared_ptr<DataMsg>> LineDBManager::queryLineFromDB(DataType type, bool ignoreStatus)
    {
        std::vector<std::shared_ptr<DataMsg>> vtLine;
        try
        {
            std::string table = getTableName(type);
            std::string stsCondition = ignoreStatus ? "" : " where status=0";
            SQLite::Statement  query(*m_pDB, "SELECT * FROM " + table + stsCondition + " order by id asc limit 1000"); 

            std::vector<int> vtId;
            std::vector<std::vector<int>> vtMsgIds;
            std::vector<int> vtMsgId;
            std::vector<std::string> vtRawLine;
            int accumulatedCnt = 0;
            std::string oneLine = "";
            while (query.executeStep())
            {
                int id = query.getColumn(0);
                const char* createdTime = query.getColumn(1);
                const char* packet = query.getColumn(2);
                int status = query.getColumn(3);
                int line_cnt = query.getColumn(4);
                int retry_cnt = query.getColumn(5);

                if (accumulatedCnt + line_cnt > in_constants::RUM_ITEM_LIMIT_SIZE)
                {
                    vtRawLine.push_back(oneLine);
                    accumulatedCnt = 0;
                    oneLine = "";

                    vtMsgIds.push_back(vtMsgId);
                    vtMsgId.clear();
                }
                
                vtMsgId.push_back(id);
                vtId.push_back(id);
                oneLine = oneLine + packet;
                accumulatedCnt += line_cnt;

                std::stringstream info;
                info << "id: " << id << ", line_cnt: " << line_cnt << ", acc_line: " << accumulatedCnt << std::endl;
                LoggerManager::getInstance().logDebug(info.str());
            }

            if (oneLine.size() > 0)
            {
                vtRawLine.push_back(oneLine);
                vtMsgIds.push_back(vtMsgId);
            }

            int idx = 0;
            for (auto ln : vtRawLine)
            {
                std::shared_ptr<DataMsg> dm = std::make_shared<DataMsg>();
                dm->dataType = type;
                dm->rawLine = ln;
                dm->vtId = vtMsgIds[idx];

                vtLine.push_back(dm);
                idx++;
            }

            if (ignoreStatus)
            {
                updateLineStatus(table, LineStatus::poped, vtId);
            }
        }
        catch (std::exception& e)
        {
            LoggerManager::getInstance().logError("exception: {}", e.what());
        }

        return vtLine;
    }

    void LineDBManager::putLineBackToDB(DataMsg& msg)
    {
        std::string tableName = getTableName(msg.dataType);
        updateLineStatus(tableName, LineStatus::ready, msg.vtId);
    }

    void LineDBManager::deleteLineFromDB(DataMsg& msg)
    {
        std::string tableName = getTableName(msg.dataType);
        deleteLines(tableName, msg.vtId);
    }

    int LineDBManager::deleteLines(const std::string table, std::vector<int> vtId)
    {
        // delete the retrieved items
        return executeSQLs("delete from " + table + " where id=?", vtId);
    }

    int LineDBManager::updateLineStatus(const std::string table, LineStatus status, std::vector<int> vtId)
    {
        return executeSQLs("update " + table + " set status=" + std::to_string(status) + " where id=?", vtId);
    }

    int LineDBManager::executeSQL(std::string sql)
    {
        LoggerManager::getInstance().logTrace("Execute SQL: " + sql);
        return m_pDB->exec(sql);
    }

    int LineDBManager::executeSQLs(std::string sql, std::vector<int> vtIds)
    {
        SQLite::Statement query{ *m_pDB, sql };

        int cnt = 0;
        for (const auto& v : vtIds)
        {
            query.bind(1, v);
            cnt += query.exec();
            query.reset();
        }

        if (cnt > 0)
        {
            LoggerManager::getInstance().logDebug("Execute SQL: " + sql);
        }

        return cnt;
    }

    std::string LineDBManager::getTableName(DataType type)
    {
        std::string tableName = "";

        switch (type)
        {
        case DataType::RUM_APP:
        case DataType::RUM_WEBVIEW:
            tableName = "line_rum";
            break;
        case DataType::LOG:
            tableName = "line_log";
            break;
        case DataType::TRACE:
            tableName = "line_trace";
            break;
        default:
            break;
        }

        return tableName;
    }

    /**
     * reserve room for adding log.
     *
     * @param newCnt - how many new logs to be added
     * @return		 - how many new logs need to be removed before adding
     */
    int LineDBManager::reserveRoom4Log(int newCnt)
    {
        int removeCnt = 0;
        int logCount = getLineCount(DataType::LOG);
        if (logCount >= m_userLogCountThreshold)
        {
            if (m_logRetentionPolicy == LogCacheDiscard::DISCARD)
            {
                return newCnt;
            }
            else
            {
                removeCnt = logCount - m_userLogCountThreshold + newCnt;
            }
        }
        else if (logCount + newCnt <= m_userLogCountThreshold)
        {
            return 0;
        }
        else
        {
            int tobeRemove = logCount + newCnt - m_userLogCountThreshold;
            if (logCount < m_userLogCountThreshold)
            {

            }
            if (m_logRetentionPolicy == LogCacheDiscard::DISCARD)
            {
                return (logCount + newCnt - m_userLogCountThreshold);
            }
            else
            {
                removeCnt = logCount + newCnt - m_userLogCountThreshold;
            }
        }

        if (removeCnt > 0)
        {
            // clear the oldest db log
            deleteOldestLogs(removeCnt);
        }

        return 0;
    }

    int LineDBManager::deleteOldestLogs(int count)
    {
        try
        {
            std::string sql = "DELETE FROM line_log WHERE id IN(SELECT id FROM line_log ORDER BY id ASC LIMIT ";
            sql = sql + std::to_string(count) + ")";

            int rows = executeSQL(sql);
            return rows;
        }
        catch (std::exception& e)
        {
            LoggerManager::getInstance().logError("SQLite exception during deletion: {}", e.what());
        }

        return 0;
    }
}