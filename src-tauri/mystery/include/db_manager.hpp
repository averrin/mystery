#pragma once
#include "fmt/format.h"
#include <iostream>
#include <vector>

const auto DB_NAME = "city.db";

class DbManager {
private:
  // SQLite::Database db;

public:
  DbManager() {} // : db(DB_NAME, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {}

  void createTable(std::string tableName, std::vector<std::string> fields) {
    return;
    // std::cout << "SQlite3 version " << SQLite::VERSION << " ("
    //           << SQLite::getLibVersion() << ")" << std::endl;
    // std::cout << "SQliteC++ version " << SQLITECPP_VERSION << std::endl;
    // const SQLite::Header header = SQLite::Database::getHeaderInfo(DB_NAME);

    // // Print values for all header fields
    // // Official documentation for fields can be found here:
    // // https://www.sqlite.org/fileformat.html#the_database_header
    // std::cout << "Magic header string: " << header.headerStr << std::endl;
    // std::cout << "Page size bytes: " << header.pageSizeBytes << std::endl;
    // std::cout << "File format write version: "
    //           << (int)header.fileFormatWriteVersion << std::endl;
    // std::cout << "File format read version: "
    //           << (int)header.fileFormatReadVersion << std::endl;
    // std::cout << "Reserved space bytes: " << (int)header.reservedSpaceBytes
    //           << std::endl;
    // std::cout << "Max embedded payload fraction "
    //           << (int)header.maxEmbeddedPayloadFrac << std::endl;
    // std::cout << "Min embedded payload fraction: "
    //           << (int)header.minEmbeddedPayloadFrac << std::endl;
    // std::cout << "Leaf payload fraction: " << (int)header.leafPayloadFrac
    //           << std::endl;
    // std::cout << "File change counter: " << header.fileChangeCounter
    //           << std::endl;
    // std::cout << "Database size pages: " << header.databaseSizePages
    //           << std::endl;
    // std::cout << "First freelist trunk page: " << header.firstFreelistTrunkPage
    //           << std::endl;
    // std::cout << "Total freelist trunk pages: " << header.totalFreelistPages
    //           << std::endl;
    // std::cout << "Schema cookie: " << header.schemaCookie << std::endl;
    // std::cout << "Schema format number: " << header.schemaFormatNumber
    //           << std::endl;
    // std::cout << "Default page cache size bytes: "
    //           << header.defaultPageCacheSizeBytes << std::endl;
    // std::cout << "Largest B tree page number: " << header.largestBTreePageNumber
    //           << std::endl;
    // std::cout << "Database text encoding: " << header.databaseTextEncoding
    //           << std::endl;
    // std::cout << "User version: " << header.userVersion << std::endl;
    // std::cout << "Incremental vaccum mode: " << header.incrementalVaccumMode
    //           << std::endl;
    // std::cout << "Application ID: " << header.applicationId << std::endl;
    // std::cout << "Version valid for: " << header.versionValidFor << std::endl;
    // std::cout << "SQLite version: " << header.sqliteVersion << std::endl;

    // auto _q = "DROP TABLE IF EXISTS {}";
    // auto q = fmt::format(fmt::runtime(_q), tableName);
    // fmt::print("query: {}\n", q);
    // this->db.exec(q);
    // fmt::print("query: {} -> done\n", q);

    // _q = "CREATE TABLE {} ({})";
    // q = fmt::format(fmt::runtime(_q), tableName, fmt::join(fields, ", "));
    // fmt::print("query: {}\n", q);
    // this->db.exec(q);
    // fmt::print("query: {} -> done\n", q);
  }
};
