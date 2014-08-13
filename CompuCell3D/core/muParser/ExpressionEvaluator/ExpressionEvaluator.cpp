#include <ostream>
#include <vector>
#include <map>
#include <string>

#include <muParser.h>
#include <limits>
#include <XMLUtils/CC3DXMLElement.h>

#include "ExpressionEvaluator.h"

using namespace mu;
using namespace std;

ExpressionEvaluatorDepot::ExpressionEvaluatorDepot ( unsigned int _size )
{
    eeVec.assign ( _size,ExpressionEvaluator() );
}

//unsafe but fast
ExpressionEvaluator & ExpressionEvaluatorDepot::operator [] ( unsigned int _idx )
{
    return eeVec[_idx];
}

void ExpressionEvaluatorDepot::addVariable ( string _name )
{
    for ( auto &elem : eeVec )
        elem.addVariable ( _name );
}

void ExpressionEvaluatorDepot::setAlias ( std::string _name, std::string _alias )
{
    for ( auto &elem : eeVec )
        elem.setAlias ( _name, _alias );
}

void ExpressionEvaluatorDepot::setExpression ( std::string _expression )
{
    expressionString=_expression;
    for ( auto &elem : eeVec )
        elem.setExpression ( expressionString );
}

void ExpressionEvaluatorDepot::allocateSize ( unsigned int _size )
{
    eeVec.clear();
    eeVec.assign ( _size,ExpressionEvaluator() );
}

void ExpressionEvaluatorDepot::getParseData ( CC3DXMLElement *_xmlData )
{
    parseData=ExpressionEvaluatorDepotParseData(); //new copy of parse data

    CC3DXMLElementList variableVecXML=_xmlData->getElements ( "Variable" );
	for (const auto& elem : variableVecXML)
	{
		parseData.varNameVec.push_back ( elem->getAttribute ( "Name" ) );
        parseData.varValueVec.push_back( elem->getAttributeAsDouble ( "Value" ) );
	}

    CC3DXMLElementList builtinVariableVecXML=_xmlData->getElements ( "BuiltinVariable" );
	for (const auto& elem : builtinVariableVecXML)
		parseData.builtinVariableAliasPairVec.push_back ( make_pair ( elem->getAttribute ( "builtinName" ),elem->getAttribute ( "Alias" ) ) );

	parseData.expression=_xmlData->getFirstElement ( "Expression" )->getText();
}

void ExpressionEvaluatorDepot::initializeUsingParseData()
{
    //define variables for parsers
    addVariables ( parseData.varNameVec.begin(),parseData.varNameVec.end() );
    //set variables' values for parsers
    setVariables ( parseData.varValueVec.begin(),parseData.varValueVec.end(),this->getNumberOfVars() );
    //set aliases
    for ( const auto &elem : parseData.builtinVariableAliasPairVec )
        setAlias ( elem.first, elem.second );

    //set expression
    if ( parseData.expression.size() )   //we only set expression if the expression string has some content
        setExpression ( parseData.expression );
}

void ExpressionEvaluatorDepot::update ( CC3DXMLElement *_xmlData, bool _fullInitFlag )
{
    if ( !_xmlData )
        return;

    vector<string> varNameVec;
    vector<double> varValueVec;
    vector<pair<string,string> > builtinVariableAliasPairVec;
    string expression;

    CC3DXMLElementList variableVecXML=_xmlData->getElements ( "Variable" );
    for ( const auto& elem : variableVecXML )
    {
        varNameVec.push_back ( elem->getAttribute ( "Name" ) );
        varValueVec.push_back ( elem->getAttributeAsDouble ( "Value" ) );
    }

    CC3DXMLElementList builtinVariableVecXML=_xmlData->getElements ( "BuiltinVariable" );
    for ( const auto& elem : builtinVariableVecXML )
        builtinVariableAliasPairVec.push_back ( make_pair ( elem->getAttribute ( "builtinName" ),elem->getAttribute ( "Alias" ) ) );

    unsigned int numberOfExistingVariables=this->getNumberOfVars(); // these variables were addedbefore calling this fcn
    //define variables for parsers
    addVariables ( varNameVec.begin(),varNameVec.end() );
    //set variables' values for parsers
    setVariables ( varValueVec.begin(),varValueVec.end(),numberOfExistingVariables );
    //setVariables(varValueVec.begin(),varValueVec.end(),this->getNumberOfVars());
    //set aliases
    for ( auto &elem : builtinVariableAliasPairVec )
        setAlias ( elem.first, elem.second );

    //set expression
    expression=_xmlData->getFirstElement ( "Expression" )->getText();
    setExpression ( expression );
}

ExpressionEvaluator::ExpressionEvaluator()
{}

void ExpressionEvaluator::setExpression ( std::string _expression )
{
    try
    {
        expressionString=_expression;
        p.SetExpr ( expressionString );
    }
    catch ( Parser::exception_type &e )
    {
        cerr << "setExpression Function: "<<e.GetMsg() << endl;
    }
}

void ExpressionEvaluator::setAlias ( std::string _name, std::string _alias )
{
    nameToAliasMap[_name]=_alias;
    auto mitr = varNameToIndexMap.find ( _name );
    if ( mitr==varNameToIndexMap.end() )
    {
        string errorStr="Variable "+_name+" undefined. Please define it before defining an alias to it";
        throw mu::ParserError ( errorStr.c_str(), 0, "" );
    }
    else
    {
        auto mitr_local = varNameToIndexMap.find ( _alias );
        if ( mitr_local!=varNameToIndexMap.end() )
        {
            string errorStr="Proposed alias: "+_alias+" already exists as another variable. Please change alias name";
            throw mu::ParserError ( errorStr.c_str(), 0, "" );
        }
        p.DefineVar ( _alias,&varVec[mitr->second] );
        varNameToIndexMap[_alias]=mitr->second;
    }
}

void ExpressionEvaluator::addVariable ( std::string _name )
{
    auto mitr = varNameToIndexMap.find ( _name );
    if ( mitr==varNameToIndexMap.end() )
    {
        varVec.push_back ( 0.0 );
        varNameToIndexMap.insert ( make_pair ( _name,varVec.size()-1 ) );
        //after adding new variable we have to reinitialize variable locations that pparser has . in practice it means creating new parser and reinitializing it using varNameToIndexMap
        p=Parser();
        for ( const auto& elem : varNameToIndexMap )
        {
			cerr<<"associating "<<elem.first<<" with index "<<elem.second<<endl;
            p.DefineVar ( elem.first, &varVec[elem.second] );
        }
        if ( expressionString.size() )
        {
            p.SetExpr ( expressionString );
        }
    }
}

void ExpressionEvaluator::setVar ( std::string _name, double _val )
{
    auto mitr = varNameToIndexMap.find ( _name );

    if ( mitr!=varNameToIndexMap.end() )
    {
        varVec[mitr->second]=_val;
    }
}

//unsafe but fast
void ExpressionEvaluator::setVarDirect ( unsigned int _idx, double _val )
{
    varVec[_idx]=_val;
}

//unsafe but fast
double & ExpressionEvaluator::operator [] ( unsigned int _idx )
{
    return varVec[_idx];
}

double ExpressionEvaluator::getVar ( std::string _name )
{
    auto mitr = varNameToIndexMap.find ( _name );
    if ( mitr!=varNameToIndexMap.end() )
    {
        return varVec[mitr->second];
    }
    else
    {
        return numeric_limits<double>::quiet_NaN(); //NaN
    }
}

//unsafe but fast
double ExpressionEvaluator::getVarDirect ( unsigned int _idx )
{
    return ( *this ) [_idx];
}

double ExpressionEvaluator::eval()
{
    return p.Eval();
}

// Function callback
double MySqr ( double a_fVal )
{
    return a_fVal*a_fVal;
}

// main program
int main ( int argc, char* argv[] )
{
    using namespace mu;

    ExpressionEvaluatorDepot eed;
    vector<string> varNames1 ( 2 );
    varNames1[0]="a";
    varNames1[1]="b";

    eed.allocateSize ( 3 );
    eed.addVariables ( varNames1.begin(),varNames1.end() );

    eed.setExpression ( "a+b" );
    for ( unsigned int i =  0 ; i < eed.size() ; ++i )
    {
        ExpressionEvaluator &ev=eed[i];
        ev[0]=i*10;
        ev[1]=i*12;

		cerr<<"i="<<i<<" x="<<ev.eval() <<endl;
    }

    ExpressionEvaluator ev;
    vector<string> varNames ( 2 );
    varNames[0]="a";
    varNames[1]="b";
    ev.addVariables ( varNames.begin(),varNames.end() );
    ev.setAlias ( "a","abc" );

    //ev.addVariable("a");
    //ev.addVariable("b");
    ev.setExpression ( "(a>11) ? (abc+b) : (abc-b)" );
    ev[0]=9;
    ev[1]=12;

    const Parser &p = ev.getMuParserObject();
    cerr<<"this is var [0]="<<ev[0]<<endl;
    cerr<<p.Eval() <<endl;
}
