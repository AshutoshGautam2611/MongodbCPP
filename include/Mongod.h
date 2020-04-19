#ifndef MONGOD_H
#define MONGOD_H
using namespace std;
class Client;
class Database;
class Document;
class CollectionIterator
{
private:
list<Document> * collectionsData;
list<Document>::iterator itr;
int size;
int track;
public:
CollectionIterator(list<Document > * documents);
Document next();
int hasNext();
};
class DataBag
{
private:
string value;
string dataType;
public:
DataBag(string value,string dataType);
~DataBag();
string getValue();
string getDataType();

};
class Document
{
private:
map<string,DataBag*> * elements;
DataBag * dataBag;
map<string,DataBag *> * getElements();
string jsonString;
list<string> keys;
list<string> values;
void setKeys(list<string> keys);
void setValues(list<string> values);

public:
Document();
~Document();
void append(string key,string value);
void append(string key, int value);
void append(string key, float value);
void append(string key, double value);
void append(string key , long value);
void setJson(string json);
string getJson();
list<string> getKeys();
list<string> getValues();

//getKey list
//get values list
friend class Collection;
};
class Collection
{
private:
Database * database;
Client *client;
string name;
mongoc_collection_t * collection;
public:
Collection(Client * client,Database * database,string name);
~Collection();
int add(Document document);
int remove(Document document);
int removeAll();
int update(Document queryDocument,Document document);
CollectionIterator iterate();
};
class Database
{
private:
Client *client;
list<string> *collections;
string name;
mongoc_database_t *database;
const char * getDatabaseName();
public:
Database(Client * client,string name);
~Database();
void initalizeCollections();
Collection getCollection(string collectionName);
list<string> * getCollections();
friend class Collection;
};
class Client
{
private :
int port;
string host;
bool isSecure;
string username;
string password;
mongoc_client_t * mongoClient;
list<string> * databases;
string connectionString;
mongoc_client_t * getMongoClient();
public:
Client(string host, int port);
~Client();
Client(string host, int port, bool isSecure, string username , string password);
Database getDatabase(string databaseName);
void initalizeDatabase();
list<string> * getDatabases();
void setConnectionString(string connectionString);
string getConnectionString();
friend class Database;
friend class Collection;
};

#endif
