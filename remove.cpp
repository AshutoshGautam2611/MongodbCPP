#include<iostream>
#include "include//MongoLib.cpp"
int main()
{
Client client("localhost",27017);
Database database=client.getDatabase("tmdb");
Collection collection=database.getCollection("students");
Document queryDoc;
queryDoc.append("_id",309);
if(collection.remove(queryDoc))
{
std::cout << "deleted" << '\n';
}
else
{
std::cout << "Error occurs" << '\n';
}
}
