#include <stdlib.h>
#include <iostream>

#include "jdbc/mysql_connection.h"

#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>

//using namespace std;

int main(void)
{
	std::string names[5] = { "Pavel","John","Ivan","Patrick","Inna" };
	std::string surnames[5] = { "Taran","Wick","Ivanov","Bateman","Petrova" };
	try {
		sql::Driver* driver;
		sql::Connection* con;
		sql::Statement* stmt;
		sql::ResultSet* res;
		sql::PreparedStatement* pstmt;

		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("localhost", "root", "1234");
		/* Connect to the MySQL database */
		stmt = con->createStatement();
		stmt->execute("DROP DATABASE IF EXISTS db_test");
		stmt->execute("CREATE DATABASE db_test");
		con->setSchema("db_test");
		stmt->execute("DROP TABLE IF EXISTS people");
		stmt->execute("CREATE TABLE people (id int, name varchar(32), surname varchar(32))");
		pstmt = con->prepareStatement("INSERT INTO people(id, name, surname) VALUES (?, ?, ?)");
		for (int i = 0; i < 5; i++)
		{
			pstmt->setInt(1, i + 1);
			pstmt->setString(2, names[i]);
			pstmt->setString(3, surnames[i]);
			pstmt->execute();
		
		}
		res = stmt->executeQuery("SELECT * FROM people");
		while (res->next()) {
			if (res->getString("id") == "1")
			{
				std::cout << res->getString("name") << " " << res->getString("surname") << std::endl;
			}
		}
		delete res;
		delete pstmt;
		delete stmt;
		delete con;

	}
	catch (sql::SQLException& e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
	return EXIT_SUCCESS;
}