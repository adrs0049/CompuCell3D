#include <iostream>
#include <fstream>
#include <cstdio>
#include <XMLUtils/XMLParserExpat.h>

using namespace std;

XMLParserExpat::XMLParserExpat() :
     level ( 0 ), rootElement ( nullptr )
{}

XMLParserExpat::~XMLParserExpat() {}

void XMLParserExpat::setFileName ( std::string _fileName )
{
     fileName=_fileName;
}

int XMLParserExpat::parse()
{
     //XML_Parser parserObject=XML_ParserCreate(NULL);
     XML_Parser parser=XML_ParserCreate ( NULL );
     XML_SetUserData ( parser,this ); //pass this  to our routines

     ///* Register routines to get called as the file is parsed */
     XML_SetElementHandler ( parser,
                             ( XML_StartElementHandler ) handleStartElement,
                             ( XML_EndElementHandler ) handleEndElement );
     XML_SetCharacterDataHandler ( parser,
                                   ( XML_CharacterDataHandler ) handleCharacterData );

     ifstream inputStream;
     inputStream.open ( fileName.c_str() );

     if ( !inputStream.is_open() )
          cerr<<"Error opening file "<<fileName<<endl;
     else {
          std::string line;
          bool done=false;
          long lineNumber=1;
          while ( getline ( inputStream,line ) ) {

               if ( XML_Parse ( parser, line.c_str(), line.size(), done ) == XML_STATUS_ERROR ) {
				   const char * error_string=XML_ErrorString ( XML_GetErrorCode ( parser ) );
                   cerr<<"ERROR in the XML file: "<<fileName<<" "
						<< error_string <<" in line "<< lineNumber<<endl;
				   inputStream.close(); 
                   return 1;
               }
               ++lineNumber;
          }
     }

     if ( inputStream.bad() )
          cerr<<"Error reading file"<<endl;
		  
     inputStream.close();

     XML_ParserFree ( parser );

	 // Error handling is a bit weird here
	 return 0;
}

void handleStartElement ( XMLParserExpat *_xmlExpatParser,const XML_Char *name, const XML_Char **atts )
{

     _xmlExpatParser->tag=name;
#ifdef _DEBUG
     cerr<<"Starting tag="<<_xmlExpatParser->tag<<endl;
#endif
     map<string,string> atributeDictionary;
     for ( int i=0; atts[i]; i+=2 ) {
          atributeDictionary.insert ( make_pair ( string ( atts[i] ),string ( atts[i+1] ) ) );
#ifdef _DEBUG
          cerr<<"attrName="<<atts[i]<<" attrValue="<<atts[i+1]<<endl;
#endif
     }
     CC3DXMLElement element ( string ( name ), atributeDictionary );

     _xmlExpatParser->elementInventory.push_back ( element );
     CC3DXMLElement * elementPtr=&_xmlExpatParser->elementInventory.back();

     if ( _xmlExpatParser->nodeStack.size() ) {
          _xmlExpatParser->nodeStack.top()->addChild ( elementPtr );
     } else {
          _xmlExpatParser->rootElement = elementPtr;

     }
     _xmlExpatParser->nodeStack.push ( elementPtr );
     _xmlExpatParser->level++; /* entering a new tag */
}

void handleEndElement ( XMLParserExpat *_xmlExpatParser,const XML_Char *name )
{
     _xmlExpatParser->level--; // leaving a tag
#ifdef _DEBUG
     cerr<<"Ending tag: '"<<name<<"'\n";
#endif

     _xmlExpatParser->nodeStack.pop();
}

std::string squeeze ( const std::string &str )
{
     unsigned int start=0,end=str.size();
     while ( start<end && isspace ( str[start] ) ) start++;
     while ( start<end && isspace ( str[end-1] ) ) end--;
     return std::string ( str,start,end-start );
}

void handleCharacterData ( XMLParserExpat *_xmlExpatParser,const XML_Char *data,int len )
{
     string str=squeeze ( string ( data,len ) ); /*<- silly: expat will give you whitespace too */
     if ( str.size() >0u )
#ifdef _DEBUG
          cerr<<"Character data: '"<<str<<"'\n";
#endif
     _xmlExpatParser->nodeStack.top()->setData(str);
}
