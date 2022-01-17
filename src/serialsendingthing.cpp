#include <src/DW3000_interface.h>
#include <lib/assertions.h>



/*
scedule reports to be send by calling scedule report and then them with send_sceduled_reports
you can also send a single report without affecting the scedule with send_single_report
get_repnum() tells you how many reports are sceduled
you can scedule at most REPLISTZISE reports( but shouldnt )

*/


struct stampblock
{
    uint32_t sendid; //the ids are just 1 byte but take up 4bytes of space anyway, and this makes everything way easier to send
    uint32_t recvid;
    uint64_t rxtime;
    uint64_t txtime;
};
#define REPLISTZISE 64 // at time of writing there would be enough space in RAM for 1197 stampblocks
//place to store not send reports and counter
struct stampblock replist[REPLISTZISE];
int repnum = 0;


int get_repnum(){return repnum;}

void send(uint8_t* data, int size){
    //sends size bytes 
    for(int i=0;i<size;i++){
        Serial.write(data[i]);
    }
}





void scedule_report(TRIA_RangeReport rep){
    //writes rep into replist
    VERIFY(repnum < REPLISTZISE); // just throw a error if there is no space
    //i might want to implement a ringbuffer; but again, i dont think it will ever go full
    replist[repnum].sendid = rep.sent_to().id();
    replist[repnum].recvid = rep.received_from().id();
    replist[repnum].rxtime = rep.get_rx_stamp().value();
    replist[repnum].txtime = rep.get_tx_stamp().value();

    repnum++;
    
}

int send_sceduled_reports(){
    /*  sends all reports sceduled by scedule_report() and resets the que
        returns the number of formerly sceduled reports
    */
    int ret = repnum;

    while (Serial.read() != 0xff);
    Serial.write(repnum);

    send((uint8_t*) replist, repnum*sizeof(stampblock));
    repnum = 0; //everything is send, ne reason to null anything, it'll just get overwritten or ignored next time
    return ret;

}

void send_single_report(TRIA_RangeReport rep){

    while (Serial.read() != 0xff);
    Serial.write(1);
    
    stampblock togive =  replist[REPLISTZISE -1]; //use last element as free space, this should not be a problem and i dont forsee ever filling the list anyway

    togive.sendid = rep.sent_to().id();
    togive.recvid = rep.received_from().id();
    togive.rxtime = rep.get_rx_stamp().value();
    togive.txtime = rep.get_tx_stamp().value();

    send((uint8_t*)&togive,sizeof(stampblock));


}


