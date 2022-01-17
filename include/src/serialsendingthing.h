#include <src/DW3000_interface.h>

int get_repnum();
void scedule_report(TRIA_RangeReport rep);
int send_sceduled_reports();
void send_single_report(TRIA_RangeReport rep);