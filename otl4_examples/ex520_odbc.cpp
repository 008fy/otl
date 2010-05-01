#include <iostream>
using namespace std;

#include <stdio.h>

#define OTL_FREETDS_ODBC_WORKAROUNDS
#define OTL_ODBC_MSSQL_2005 // Compile OTL 4/ODBC, MS SQL 2005
//#define OTL_ODBC // Compile OTL 4/ODBC. Uncomment this when used with MS SQL 7.0/ 2000
//#define OTL_ODBC_UNIX // Compile OTL 4 / ODBC. Uncomment this when used in Linux / Unix
#include <otlv4.h> // include the OTL 4.0 header file

otl_connect db; // connect object

void insert()
// insert rows into table
{ 
 otl_stream o(50, // buffer size
              "insert into test_tab values(:f1<float>,:f2<char[31]>)", 
                 // SQL statement
              db // connect object
             );
 char tmp[32];

 for(int i=1;i<=100;++i){
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400) // VC++ 8.0 or higher
  sprintf_s(tmp,sizeof(tmp),"Name%d",i);
#else
  sprintf(tmp,"Name%d",i);
#endif
#else
  sprintf(tmp,"Name%d",i);
#endif
  o<<(float)i<<tmp;
 }
}

void select()
{ 
 otl_stream i(50, // buffer size
              "select * from test_tab where f1>=:f11<char[40]> "
              " and f1<=:f12<char[40]>*2",
                 // SELECT statement
              db // connect object
             ); 
   // create select stream
 
 float f1;
 char f2[31];
 
 try{
  cout<<"====> Starting a fetch sequence...."<<endl;
  i<<"xxx8"<<"8"; // assigning :f11 = "xxx8", :f12 = "8", it's obviously 
             // invalid for a numeric column
    // SELECT automatically executes when all input variables are
    // assigned. First portion of output rows is fetched to the buffer

  while(!i.eof()){ // while not end-of-data
   i>>f1>>f2;
   cout<<"f1="<<f1<<", f2="<<f2<<endl;
  } 
 }catch(otl_exception& p){
  cerr<<"===> A database exception is caught: "<<endl;
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
  cerr<<"===> Cleaning up the stream's error flags"<<endl;
  i.clean();
 }

 cout<<"====> Starting another fetch sequence..."<<endl;
 i<<"4"<<"4"; // assigning :f11 = 4, :f12 = 4
   // SELECT automatically executes when all input variables are
   // assigned. First portion of output rows is fetched to the buffer

 while(!i.eof()){ // while not end-of-data
  i>>f1>>f2;
  cout<<"f1="<<f1<<", f2="<<f2<<endl;
 }

}

int main()
{
 otl_connect::otl_initialize(); // initialize the environment
 try{

  db.rlogon("scott/tiger@freetds_mssql"); // connect to the database

  otl_cursor::direct_exec
   (
    db,
    "drop table test_tab",
    otl_exception::disabled // disable OTL exceptions
   ); // drop table

  otl_cursor::direct_exec
   (
    db,
    "create table test_tab(f1 int, f2 varchar(30))"
    );  // create table

  insert(); // insert records into table
  select(); // select records from table

 }

 catch(otl_exception& p){ // intercept OTL exceptions
  cerr<<p.msg<<endl; // print out error message
  cerr<<p.stm_text<<endl; // print out SQL that caused the error
  cerr<<p.var_info<<endl; // print out the variable that caused the error
 }

 db.logoff(); // disconnect from the database

 return 0;

}
