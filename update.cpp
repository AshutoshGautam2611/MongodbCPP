#include<iostream>
#include "include//MongoLib.cpp"
int main()
{
Client client("localhost",27017);
Database database=client.getDatabase("tmdb");
Collection collection=database.getCollection("students");
Document doc;
Document queryDoc;
queryDoc.append("_id",310);
doc.append("_id",310);
doc.append("firstName","Mr. Dhananjay");
doc.append("last","Singh");
if(collection.update(queryDoc,doc))
{
std::cout << "Updated" << '\n';
}
else
{
std::cout << "Error occurs" << '\n';
}
}
