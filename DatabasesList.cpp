#include<iostream>
#include "include//MongoLib.cpp"
int main()
{
Client client("localhost",27017);
list<string> * databases=client.getDatabases()  ;
for(list<string>::iterator itr=databases->begin() ; itr!=databases->end();++itr)
{
cout<<*itr<<endl;
}
}
