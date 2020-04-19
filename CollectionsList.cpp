#include<iostream>
#include "include//MongoLib.cpp"
int main()
{
Client client("localhost",27017);
Database database=client.getDatabase("tmdb");
list<string > *collections= database.getCollections();
for(list<string>::iterator itr=collections->begin() ; itr!=collections->end();++itr)
{
cout<<*itr<<endl;
}
}
