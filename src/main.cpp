#include <iostream>

#include "Database.h"
#include "DatabaseDef.h"

int main(int argc, char **argv) {
    Database testDTB;
    testDTB.parser(argv[1]);
    cout << "Done parser!!!" << endl;
    for(int i=0;i<testDTB.getNumClkNets();++i)
    {
        for(int j=0;j<testDTB.getClkNets()[i]->numPins();++j)
        {
            if(testDTB.getClkNets()[i]->pin(j).module()==nullptr)
                continue;
            cout<<testDTB.getClkNets()[i]->pin(j).module()->name()<<" : ";
            cout<<testDTB.getClkNets()[i]->pin(j).x()<<",";
            cout<<testDTB.getClkNets()[i]->pin(j).y()<<"  ";
            cout<<testDTB.getClkNets()[i]->pin(j).getSlackInfor()->slack()<<"  ";
            cout<<testDTB.getClkNets()[i]->pin(j).oldX()<<",";
            cout<<testDTB.getClkNets()[i]->pin(j).oldY()<<"  ";
            cout<<testDTB.getClkNets()[i]->pin(j)<<"  ";
            cout<<endl;
        }
    }
    double change_x = 100;
    double change_y = 250;
    for(int i=0;i<testDTB.getNumModules();++i)
    {
        for(int j=0;j<testDTB.module(i)->totnumPins();++j)
        {
            testDTB.module(i)->pin(j)->getSlackInfor()->setOldPos(testDTB.module(i)->pin(j)->x(),testDTB.module(i)->pin(j)->y());
            testDTB.module(i)->pin(j)->setPosition(testDTB.module(i)->pin(j)->x()+change_x,testDTB.module(i)->pin(j)->y()+change_y);
            testDTB.updateSlack(testDTB.module(i)->pin(j));
        }
    }
    // for(int i=0;i<testDTB.getNumClkNets();++i)
    // {
    //     for(int j=0;j<testDTB.getClkNets()[i]->numPins();++j)
    //     {
    //         if(testDTB.getClkNets()[i]->pin(j).module()==nullptr)
    //             continue;
    //         cout<<testDTB.getClkNets()[i]->pin(j).module()->name()<<" : ";
    //         cout<<testDTB.getClkNets()[i]->pin(j).x()<<",";
    //         cout<<testDTB.getClkNets()[i]->pin(j).y()<<"  ";
    //         cout<<testDTB.getClkNets()[i]->pin(j).getSlackInfor()->slack()<<"  ";
    //         cout<<testDTB.getClkNets()[i]->pin(j).oldX()<<",";
    //         cout<<testDTB.getClkNets()[i]->pin(j).oldY()<<"  ";
    //         cout<<endl;
    //     }
    // }
    return 0;
}