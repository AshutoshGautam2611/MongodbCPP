#include<iostream>
#include "include//MongoLib.cpp"
int main()
{
Client client("localhost",27017);
Database database=client.getDatabase("tmdb");
Collection collection=database.getCollection("students");
Document doc;
doc.append("_id",310);
doc.append("firstName","Dhananjay");
doc.append("last","Singh");
if(collection.add(doc))
{
std::cout << "Added" << '\n';
}
else
{
std::cout << "Error occurs" << '\n';
}
}
