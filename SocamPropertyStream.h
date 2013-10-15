/**
 * SocamPropertyStream.h 
 * Autor: Guillermo López González
 * 
 * Copyright (c) 2013 FIDESOL
 */
#ifndef SOCAMPROPERTYSTREAM_H
#define SOCAMPROPERTYSTREAM_H

#include <fstream>
#include <iostream>
#include <list>
#include <vector>
#include <string.h>


using namespace std;

namespace SOCAMCORE {

//Status type
#define P_RESULT        short
//Type definition
#define P_TYPE          short


//Buffer sizes
#define MAX_SIZE_NAME   (size_t)255
#define MAX_SIZE_VALUE  (size_t)255
#define MAX_SIZE_LINE   (size_t)(MAX_SIZE_NAME + 4 + MAX_SIZE_VALUE)

//ERROR DEFINITION
#define P_NO_ERROR        (P_RESULT)0     //OPERATION OK
#define P_IO_ERROR        (P_RESULT)-1    //IFSTREAM IO ERROR!
#define P_NOT_OPEN        (P_RESULT)-2    //IFSTREAM NOT OPEN!
#define P_NAME_NOT_FOUND  (P_RESULT)-3    //PROPERTY NAME NOT FOUND
#define P_NULL_NAME       (P_RESULT)-4    //NULL OR EMPTY NAME
#define P_EMPTY_FILE      (P_RESULT)-5    //PROPERTY FILE IS EMPTY OR NOT CONTAINS VALID PROPERTY LINES
#define P_UNKNOWN_ERROR   (P_RESULT)-6    //UNEXCEPTED ERROR

//DEFINES PROPERTY TYPES
#define P_VOID_PTR      (P_TYPE)0
#define P_STRING        (P_TYPE)1       //DEFAULT
#define P_CHAR          (P_TYPE)2
#define P_INT           (P_TYPE)3
#define P_LONG          (P_TYPE)4
#define P_FLOAT         (P_TYPE)5
#define P_DOUBLE        (P_TYPE)6
#define P_SHORT         (P_TYPE)7
#define P_UNSIGNED      (P_TYPE)10      //EXAMPLE: unsigned int = P_INT + P_UNSIGNED


/**
 PROPERTY CLASS
 */
class Property {
    private:
        /**
         NAME
         */
        string name;
        /**
         VALUE
         */
        string value;
        /**
         TYPE
         */
        P_TYPE type; //default STRING
    
    public:
        /**
         * Property Constructor
         * @param name_p
         * @param value_p
         * @param type_p
         */
        Property(string name_p, string value_p, P_TYPE type_p);
        /**
         * Destructor
         */
        virtual ~Property();

        /**
         * Get Property Name
         * @return String
         */
        string getName();
        /**
         * Get Property Value
         * @return String
         */
        string getValue();
        /**
         * Get Property Type
         * @return P_TYPE
         */
        P_TYPE getType();

        /**
         * To Stream Out
         * @param out
         */
        void toStream(ostream & out);
        /**
         * To String Out
         * @return String
         */
        string toString();
};


/**
 * SocamPropertyStream CLASS ---> std::ifstream CLASS
 */
class SocamPropertyStream : private ifstream
{
    public:
        /**
         * Contructor
         * @param path Properties file path
         */
        SocamPropertyStream(string path);
        /**
         * Destructor
         */
        virtual ~SocamPropertyStream();

        /**
         * Rebuild class: Dispose and reopen another file
         * @param path Properties file path
         */
        void rebuild(string path);
        
        /**
         * Return true if entries list is empty.
         * Status value == EMPTY_FILE
         * @return bool
         */
        bool isEmpty();
        /**
         * Return (already && good() && is_open()) 
         * @return bool
         */
        bool isAlready();
        
        /**
         * Return the result of LAST operation!
         * (See defines)
         * @return P_RESULT
         */
        P_RESULT Status();

        /**
         * Find a property by name
         * @param name
         * @return Property
         */
        Property getProperty(string name);
        
        /**
         * Find a value of property by name
         * @param name
         * @return value
         */
        string getValue(string name);

        /**
         * To Stream Out
         * @param out
         */
        void toStream(ostream & out);
        
        /**
         * To String Out
         * @return 
         */
        string toString();


    private:
        /**
         * Init class. Read file and save formated Properties
         * @param path
         */
        void init();

        /**
         * Search a property
         * @param name
         * @return Property
         */
        Property search(string name);

        /**
         * Init buffer to '\0'
         * @param buffer
         * @param size
         */
        void init_buffer(char * buffer, size_t size);
        
        /**
         LIST OF PROPERTIES
         */
        list<Property> entries;
        
        /**
         ALREADY FLAG
         */
        bool already;
        
        /**
         STATUS FLAG
         */
        P_RESULT status;
};


}
#endif // GLGPROPERTY_H
