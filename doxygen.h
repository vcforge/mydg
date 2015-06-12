typedef QList<QCString>    StringList;
typedef QListIterator<QCString>    StringListIterator;

class StringDict : public QDict<QCString>
{
  public: 
    StringDict(uint size=17) : QDict<QCString>(size) {}
    virtual ~StringDict() {}
};

void parseInput();
