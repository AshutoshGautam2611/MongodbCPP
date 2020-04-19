#include<mongoc.h>
#include<iostream>
#include<stdio.h>
#include<map>
#include<utility>
#include<list>
#include<string.h>
#include <sstream>
#include "Mongod.h"
using namespace std;

CollectionIterator::CollectionIterator(list<Document > * documents)
{
this->collectionsData=documents;
}
Document CollectionIterator::next()
{
Document  doc=this->collectionsData->front();
this->collectionsData->pop_front();
return doc;
}
int CollectionIterator::hasNext()
{
return (!this->collectionsData->empty());
}
DataBag ::DataBag(string value,string dataType)
{
this->value=value;
this->dataType=dataType;
}
DataBag::~DataBag()
{}
string DataBag ::getValue()
{
return this->value;
}


string DataBag ::getDataType()
{
return this->dataType;
}


Document::Document()
{
elements= new map<string,DataBag*>();
}

Document::~Document()
{
elements->clear();
}

map<string,DataBag *> * Document::getElements()
{
return this->elements;
}


void Document::append(string key,string value)
{
this->dataBag= new DataBag(value,"string");
this->elements->insert(pair<string, DataBag *>(key,this->dataBag));
}


void Document::append(string key, int value)
{
this->dataBag= new DataBag(to_string(value),"int");
this->elements->insert(pair<string, DataBag *>(key,this->dataBag));
}


void Document::append(string key, float value)
{
this->dataBag= new DataBag(to_string(value),"float");
this->elements->insert(pair<string, DataBag *>(key,this->dataBag));
}


void Document::append(string key, double value)
{
this->dataBag= new DataBag(to_string(value),"double");
this->elements->insert(pair<string, DataBag *>(key,this->dataBag));
}


void Document::append(string key , long value)
{
this->dataBag= new DataBag(to_string(value),"long");
this->elements->insert(pair<string, DataBag *>(key,this->dataBag));
}


void Document :: setJson(string jsonString)
{
this->jsonString=jsonString;
}


string   Document :: getJson()
{
return this->jsonString;
}

void  Document::setKeys(list<string> keys)
{
this->keys=keys;
}
void  Document::setValues(list<string> values)
{
this->values=values;
}
list<string> Document::getKeys()
{
return this->keys;
}


list<string> Document::getValues()
{
return this->values;
}


Collection::Collection(Client * client,Database * database,string name)
{
this->client=client;
this->database=database;
this->name=name;
this->collection= mongoc_client_get_collection(this->client->getMongoClient(),this->database->getDatabaseName(),this->name.c_str());
}

Collection::~Collection()
{
mongoc_collection_destroy(collection);
}



CollectionIterator  Collection::iterate()
{
list<Document> *documents= new list<Document>();
list<string> values;
list<string> keys;
bson_t query=BSON_INITIALIZER;
mongoc_cursor_t * cursor;
cursor = mongoc_collection_find_with_opts (this->collection, &query, NULL, NULL);
Document *d;
bson_iter_t iter;
const bson_t *doc;
const bson_value_t *value;
int type =0;
char * str;
string key;
string v;
while (mongoc_cursor_next (cursor, &doc))
{
str = bson_as_json (doc, NULL);
if (bson_iter_init (&iter, doc))
{
Document d;
while (bson_iter_next (&iter))
{
key=string(bson_iter_key (&iter));
keys.push_back(key);
type=0;
value = bson_iter_value (&iter);
//BSON_APPEND_UTF8
type=value->value_type;
//v=string("valueslallalala");
/*values.push_back(v);
if(type==2)
{
d.append(key,string((char *)value->value.v_utf8.str));
}
//BSON_APPEND_INT32
if(type==16)
{
d.append(key,int(value->value.v_int32));
}
//BSON_APPEND_INT64
if(type==18)
{
d.append(key,value->value.v_int64);
}
//BSON_APPEND_DOUBLE
if(type==1)
{
d.append(key,value->value.v_double);
}
*/
if(type==2)
{
v=string((char *)value->value.v_utf8.str);
d.append(key,string((char *)value->value.v_utf8.str));
}
  //BSON_APPEND_INT32
if(type==16)
{
v=to_string(value->value.v_int32);
d.append(key,int(value->value.v_int32));
}
//BSON_APPEND_INT64
if(type==18)
{
v=to_string(value->value.v_int64);
d.append(key,value->value.v_int64);
}
//BSON_APPEND_DOUBLE
if(type==1)
{
v=to_string(value->value.v_double);
d.append(key,value->value.v_double);
}
values.push_back(v);
}
d.setJson(string(str));
d.setKeys(keys);
d.setValues(values);
documents->push_back(d);
}
bson_free(str);
}
mongoc_cursor_destroy (cursor);
CollectionIterator collectionIterator(documents);
return collectionIterator;
}


int Collection::add(Document document)
{
map<string,DataBag*> * elements=document.getElements();
bson_error_t error;
bson_t * doc;
doc= bson_new();
string key;
string value;
string dataType;
map<string,DataBag *>::iterator itr;
for ( itr =elements->begin(); itr != elements->end(); itr++ )
{
key=itr->first;
dataType=itr->second->getDataType();
value=itr->second->getValue();
stringstream convert(value);
if(dataType=="string")
{
BSON_APPEND_UTF8(doc,key.c_str(),value.c_str());
}
if(dataType=="int")
{
int val=0;
convert>>val;
BSON_APPEND_INT32(doc,key.c_str(),val);
}
if(dataType=="long")
{
long val=0;
convert>>val;
BSON_APPEND_INT64(doc,key.c_str(),val);
}
if(dataType=="double")
{
double val=0;
convert>>val;
BSON_APPEND_DOUBLE(doc,key.c_str(),val);
}
if(dataType=="float")
{
float val=0;
convert>>val;
//BSON_APPEND_DECIMAL128(doc,key.c_str(),val);
}
}
if(!mongoc_collection_insert_one(this->collection,doc,NULL,NULL,&error))
{
printf("%s\n",error.message);
bson_destroy(doc);
return 0;
}
bson_destroy(doc);
return 1;
}


int Collection::update(Document queryDocument, Document document)
{
map<string,DataBag*> * elements=document.getElements();
map<string,DataBag*> * queryElements=queryDocument.getElements();
bson_error_t error;
bson_t * doc;
bson_t * queryDoc;
doc= bson_new();
queryDoc= bson_new();
string key;
string value;
string dataType;
map<string,DataBag *>::iterator itr;
for ( itr =queryElements->begin(); itr !=queryElements->end(); itr++ )
{
key=itr->first;
dataType=itr->second->getDataType();
value=itr->second->getValue();
stringstream convert(value);
if(dataType=="string")
{
BSON_APPEND_UTF8(queryDoc,key.c_str(),value.c_str());
}
if(dataType=="int")
{
int val=0;
convert>>val;
BSON_APPEND_INT32(queryDoc,key.c_str(),val);
}
if(dataType=="long")
{
long val=0;
convert>>val;
BSON_APPEND_INT64(queryDoc,key.c_str(),val);
}
if(dataType=="double")
{
double val=0;
convert>>val;
BSON_APPEND_DOUBLE(queryDoc,key.c_str(),val);
}
if(dataType=="float")
{
float val=0;
convert>>val;
//BSON_APPEND_DECIMAL128(queryDoc,key.c_str(),val);
}
}
for ( itr =elements->begin(); itr != elements->end(); itr++ )
{
key=itr->first;
dataType=itr->second->getDataType();
value=itr->second->getValue();
stringstream convert(value);
if(dataType=="string")
{
BSON_APPEND_UTF8(doc,key.c_str(),value.c_str());
}
if(dataType=="int")
{
int val=0;
convert>>val;
BSON_APPEND_INT32(doc,key.c_str(),val);
}
if(dataType=="long")
{
long val=0;
convert>>val;
BSON_APPEND_INT64(doc,key.c_str(),val);
}
if(dataType=="double")
{
double val=0;
convert>>val;
BSON_APPEND_DOUBLE(doc,key.c_str(),val);
}
if(dataType=="float")
{
float val=0;
convert>>val;
//BSON_APPEND_DECIMAL128(doc,key.c_str(),val);
}
}
if(!mongoc_collection_update(this->collection,MONGOC_UPDATE_NONE,queryDoc,doc,NULL,&error))
{
printf("%s\n",error.message);
bson_destroy(doc);
bson_destroy(queryDoc);
return 0;
}
bson_destroy(doc);
bson_destroy(queryDoc);
return 1;
}


int Collection::remove(Document document)
{
map<string,DataBag*> * elements=document.getElements();
bson_error_t error;
bson_t * queryDoc;
queryDoc= bson_new();
string key;
string value;
string dataType;
map<string,DataBag *>::iterator itr;
for ( itr =elements->begin(); itr !=elements->end(); itr++ )
{
key=itr->first;
dataType=itr->second->getDataType();
value=itr->second->getValue();
stringstream convert(value);
if(dataType=="string")
{
BSON_APPEND_UTF8(queryDoc,key.c_str(),value.c_str());
}
if(dataType=="int")
{
int val=0;
convert>>val;
BSON_APPEND_INT32(queryDoc,key.c_str(),val);
}
if(dataType=="long")
{
long val=0;
convert>>val;
BSON_APPEND_INT64(queryDoc,key.c_str(),val);
}
if(dataType=="double")
{
double val=0;
convert>>val;
BSON_APPEND_DOUBLE(queryDoc,key.c_str(),val);
}
if(dataType=="float")
{
float val=0;
convert>>val;
//BSON_APPEND_DECIMAL128(queryDoc,key.c_str(),val);
}
}
if(!mongoc_collection_remove(this->collection,MONGOC_REMOVE_NONE,queryDoc,NULL,&error))
{
printf("%s\n",error.message);
bson_destroy(queryDoc);
return 0;
}
bson_destroy(queryDoc);
return 1;
}


int Collection::removeAll()
{
bson_t queryDoc=BSON_INITIALIZER;
bson_error_t error;
if(!mongoc_collection_remove(this->collection,MONGOC_REMOVE_NONE,&queryDoc,NULL,&error))
{
printf("%s\n",error.message);
return 0;
}
return 1;
}
Database::Database(Client * client,string name)
{
this->name=name;
this->client=client;
collections= new list<string>();
database = mongoc_client_get_database (this->client->getMongoClient(), (this->name).c_str());
initalizeCollections();
}


Database::~Database()
{
mongoc_database_destroy(this->database);
}


const char * Database::getDatabaseName()
{
return (this->name).c_str();
}


void Database::initalizeCollections()
{
bson_error_t error;
bson_t opts = BSON_INITIALIZER;
char **strv;
unsigned i;
if ((strv = mongoc_database_get_collection_names_with_opts (this->database, &opts, &error)))
{
for (i = 0; strv[i]; i++)
{
collections->push_back(strv[i]);
}
bson_strfreev (strv);
}
else
{
fprintf (stderr, "Command failed: %s\n", error.message);
}
bson_destroy (&opts);
}


Collection Database::getCollection(string collectionName)
{
Collection * collection;
string * val;
int found=0;
for(list<string>::iterator itr=collections->begin();itr!=collections->end();++itr)
{
val = new string(*itr);
if(strcmp(val->c_str(),collectionName.c_str())==0)
{
found=1;
break;
}
}
if(found==1)
{
collection= new Collection(this->client,this,collectionName);
}
else
{
collection= new Collection(this->client,this,collectionName);
collections->push_back(collectionName);
}
return *collection;
}
list<string> * Database::getCollections()
{
return this->collections;
}



Client::Client(string host, int port)
{
this->port=port;
this->host=host;
this->isSecure=false;
this->databases= new list<string>();
this->connectionString="mongodb://"+host+":"+to_string(port);
mongoc_init();
this->mongoClient= mongoc_client_new(connectionString.c_str());
initalizeDatabase();
}


Client::Client(string host, int port, bool isSecure, string username , string password)
{
this->port=port;
this->host=host;
this->isSecure=true;
this->username=username;
this->password=password;
connectionString="mongodb://"+host+":"+to_string(port);
mongoClient= mongoc_client_new(connectionString.c_str());
mongoc_init();
}


mongoc_client_t * Client::getMongoClient()
{
return this->mongoClient;
}

Client :: ~Client()
{
mongoc_client_destroy(this->mongoClient);
mongoc_cleanup();
}


Database Client::getDatabase(string database)
{
Database * db;
string * val;
int found=0;
for(list<string>::iterator itr=databases->begin();itr!=databases->end();++itr)
{
val = new string(*itr);
if(strcmp(val->c_str(),database.c_str())==0)
{
found=1;
break;
}
}
if(found==1)
{
db= new Database(this,database);
}
else
{
db= new Database(this,database);
databases->push_back(database);
}
return *db;
}


void Client::initalizeDatabase()
{
bson_error_t error;
char **strv;
unsigned i;
if((strv = mongoc_client_get_database_names_with_opts (this->mongoClient, NULL, &error)))
{
for (i = 0; strv[i]; i++)
{
databases->push_back(strv[i]);
}
bson_strfreev (strv);
}
else
{
fprintf (stderr, "Command failed: %s\n", error.message);
}
}
list<string>*  Client::getDatabases()
{
return (this->databases);
}


void Client::setConnectionString(string connectionString)
{
this->connectionString=connectionString;
}


string Client::getConnectionString()
{
return this->connectionString;
}
