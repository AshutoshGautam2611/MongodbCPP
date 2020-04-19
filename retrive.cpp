#include<iostream>
#include "include//MongoLib.cpp"
int main()
{
Client client("localhost",27017);
Database database=client.getDatabase("tmdb");
Collection collection=database.getCollection("students");
CollectionIterator iterator=collection.iterate();
list<string>  keys;
list<string>  values;
while(iterator.hasNext())
{
Document  d=iterator.next();
std::cout << string(d.getJson())<< '\n';
keys=d.getKeys();
cout<<"@###KEYS -->>";
for(list<string>::iterator itr=keys.begin() ; itr!=keys.end();++itr)
{
cout<<*itr<<" -> ";
}
cout<<'\n';
std::cout << "" << '\n';
values=d.getValues();
cout<<"@###VALUES -->>";
for(list<string>::iterator itr=values.begin() ; itr!=values.end();++itr)
{
cout<<*itr<<" -> ";
}
std::cout << "\n\n\n" << '\n';
}
}
