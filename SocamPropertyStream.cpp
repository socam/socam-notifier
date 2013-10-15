#include "SocamPropertyStream.h"

namespace SOCAMCORE {

//PROPERTY CLASS
//--------------
Property::Property(string name_p, string value_p, P_TYPE type_p) {
    this->name=name_p;
    this->value=value_p;
    this->type=type_p;
}

Property::~Property() {

}

string Property::getName() {
    return this->name;
}

string Property::getValue() {
    return this->value;
}

P_TYPE Property::getType() {
    return this->type;
}

void Property::toStream(ostream & out) {
    out<<this->name<<" = "<<this->value<<endl;
}

string Property::toString() {
    return (this->name+" = "+this->value+"\r\n");
}



//SocamPropertyStream
//-----------------
/** Default constructor */
SocamPropertyStream::SocamPropertyStream(string path) : ifstream(path.c_str(), ios::in){
    if (!this->is_open()) {
        this->status=P_NOT_OPEN;
        this->already=false;
        cerr<<"SocamPropertyStream::constructor >> BASE IFSTREAM NOT OPEN ERROR!!"<<endl;
        return;
    }
        
    this->status=P_NO_ERROR;
    this->already=true;
    
    this->init();
}

/** Default destructor */
SocamPropertyStream::~SocamPropertyStream() {
    this->entries.clear();
}

bool SocamPropertyStream::isAlready() {
    return (this->already && this->eof() && this->is_open());
}

bool SocamPropertyStream::isEmpty() {
    return this->entries.empty();
}

P_RESULT SocamPropertyStream::Status() {
    return this->status;
}

Property SocamPropertyStream::getProperty(string name) {
    return this->search(name);
}

string SocamPropertyStream::getValue(string name) {
    return this->search(name).getValue();
}

void SocamPropertyStream::toStream(ostream & out) {
    if (!this->isEmpty()) {
        list<Property>::iterator it=this->entries.begin();
        for (; it != this->entries.end(); it++) {
            it->toStream(out);
        }
    }
}

string SocamPropertyStream::toString() {
    string str="";
    if (!this->isEmpty()) {
        list<Property>::iterator it=this->entries.begin();
        for (; it != this->entries.end(); it++) {
            str+=it->toString();
        }
    }
    return str;
}

void SocamPropertyStream::init() {
    if (!this->is_open()) {
        cerr<<"SocamPropertyStream::init >> Base ifstream NOT OPEN ERROR!! (Maybe invalid path..)"<<endl;
        this->status=P_NOT_OPEN;
        already=false;
        return;
    }

    try {
        char * buffer=new char[MAX_SIZE_LINE]; 
       
        while (!this->eof()) {
            init_buffer(buffer,MAX_SIZE_LINE);
            this->getline(buffer,MAX_SIZE_LINE, '\n');
            string line(buffer);
            
            if (line.empty() || line.size()>MAX_SIZE_LINE || line[0]=='#' || line[0]==' ')
                continue;
            

            std::vector<std::string> strs;
           
              const char* p;

              for (p = strtok( (char*)line.c_str(), " " );  p; )
              {
                string str = p;
                strs.push_back(str);
                p=strtok(NULL," ");
              }
            
            
            if(!strs.empty() && strs.size()==3) {
                Property p(strs[0],strs[2],P_STRING);
                entries.push_back(p);
            }
            else {
                //not compute this line of property file
                cerr<<"SocamPropertyStream::init >> Invalid line property!.. Skipping!"<<endl;
            }
        }
        
        delete [] buffer;
        if (isEmpty())  status=P_EMPTY_FILE;
        else            status=P_NO_ERROR;
        already=true;
    }
    catch (std::exception & ex) {
        this->status=P_IO_ERROR;
        already=false;
        cerr<<"SocamPropertyStream::init() >> ERROR: "<<ex.what()<<endl;
    }

}

Property SocamPropertyStream::search(string name) {
    if (name=="")       status=P_NULL_NAME;
    if (isEmpty())      status=P_EMPTY_FILE;
          
    if (!already || status!=P_NO_ERROR) {
        cerr<<"SocamPropertyStream::search(" << name <<") Class status: error! Cannot find!"<<endl;
        return Property("","",P_STRING);
    }
    
    list<Property>::iterator it=entries.begin();
    bool found=false;
    while (!found && it!=entries.end()) {
        if (it->getName()==name)
            found=true;
        else 
            it++;
    }
    
    if (found) {
        status=P_NO_ERROR;
        return *it;
    }
    status=P_NAME_NOT_FOUND;
    cerr<<"SocamPropertyStream::search >> PROPERTY NOT FOUND!"<<endl;
    return Property("","",P_STRING);
}


 void SocamPropertyStream::init_buffer(char * buffer, size_t size) {
     for (unsigned int i=0;i<size;i++)
         buffer[i]='\0';
 }

 
}
