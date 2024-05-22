#include "MESI_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;

/*************************
 * Constructor/Destructor.
 *************************/
MESI_protocol::MESI_protocol (Hash_table *my_table, Hash_entry *my_entry)
    : Protocol (my_table, my_entry)
{
    this->state=MESI_CACHE_I;
}

MESI_protocol::~MESI_protocol ()
{    
}

void MESI_protocol::dump (void)
{
    const char *block_states[9] = {"X","I","ISE","IM","S","SM","E","EM","M"};
    fprintf (stderr, "MESI_protocol - state: %s\n", block_states[state]);
}

void MESI_protocol::process_cache_request (Mreq *request)
{
    switch (state) {
    case MESI_CACHE_I: do_cache_I(request); break;
    case MESI_CACHE_ISE: do_cache_ISE(request); break;
    case MESI_CACHE_IM: do_cache_IM(request); break;
    case MESI_CACHE_S: do_cache_S(request); break;
    case MESI_CACHE_SM: do_cache_SM(request); break;
    case MESI_CACHE_E: do_cache_E(request); break;
    case MESI_CACHE_EM: do_cache_EM(request); break;
    case MESI_CACHE_M: do_cache_M(request); break;
    default:
        fatal_error ("Invalid Cache State for MESI Protocol\n");
    }
}

void MESI_protocol::process_snoop_request (Mreq *request)
{
    switch (state) {
    case MESI_CACHE_I: do_snoop_I(request); break;
    case MESI_CACHE_ISE: do_snoop_ISE(request); break;
    case MESI_CACHE_IM: do_snoop_IM(request); break;
    case MESI_CACHE_S: do_snoop_S(request); break;
    case MESI_CACHE_SM: do_snoop_SM(request); break;
    case MESI_CACHE_E: do_snoop_E(request); break;
    case MESI_CACHE_EM: do_snoop_EM(request); break;
    case MESI_CACHE_M: do_snoop_M(request); break;
    default:
        fatal_error ("Invalid Cache State for MESI Protocol\n");
    }
}

inline void MESI_protocol::do_cache_I (Mreq *request)
{
    
    switch (request->msg){
        case LOAD:
            state = MESI_CACHE_ISE;
            Sim->cache_misses++;
            send_GETS(request->addr);
            break;

        case STORE:
            state = MESI_CACHE_IM;
            Sim->cache_misses++;
            send_GETM(request->addr);
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_ISE (Mreq *request)
{
    switch (request->msg){
        case LOAD:
        case STORE:
            fatal_error ("Client: ISE state shouldn't see this message\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: ISE state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_IM (Mreq *request)
{
    switch (request->msg){
        case LOAD:
        case STORE:
            fatal_error ("Client: IM state shouldn't see this message\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: IM state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_S (Mreq *request)
{
    switch (request->msg){
        case LOAD:
            send_DATA_to_proc(request->addr);
            break;
        case STORE:
            send_GETM(request->addr);
            state = MESI_CACHE_SM;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: S state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_SM (Mreq *request)
{
    switch (request->msg){
        case LOAD:
        case STORE:
            fatal_error ("Client: SM state shouldn't see this message\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: SM state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_E (Mreq *request)
{
    switch (request->msg){
        case LOAD:
            send_DATA_to_proc(request->addr);
            break;
        case STORE:
            send_DATA_to_proc(request->addr);
            state = MESI_CACHE_M;
            
            Sim->silent_upgrades++;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: E state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_EM (Mreq *request)
{
    switch (request->msg){
        case LOAD:
        case STORE:
            fatal_error ("Client: EM state shouldn't see this message\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: EM state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_M (Mreq *request)
{
    switch (request->msg){
        case LOAD:
        case STORE:
            send_DATA_to_proc(request->addr);
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: M state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_I (Mreq *request)
{
    switch (request->msg) {
    case GETS:
    case GETM:
    case DATA:
        // Do nothing since we are already in I state
        break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: I state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_ISE (Mreq *request)
{
    switch (request->msg) {
    case GETS:
    case GETM:
        break;
    case DATA:
        state = MESI_CACHE_E;
        send_DATA_to_proc(request->addr);
        if(get_shared_line()){
            state = MESI_CACHE_S;
        }
        break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: IS state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_IM (Mreq *request)
{
    switch (request->msg) {
    case GETS:
    case GETM:
        break;
    case DATA:
        state = MESI_CACHE_M;
        send_DATA_to_proc(request->addr);
        break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: IM state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_S (Mreq *request)
{
    switch (request->msg) {
    case GETS:
        set_shared_line();
        break;
    case GETM:
        set_shared_line();
        state=MESI_CACHE_I;
        break;
    case DATA:
        send_DATA_to_proc(request->addr);
        break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: S state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_SM (Mreq *request)
{
    switch (request->msg) {
    case GETS:
        break;
    case GETM:
        break;
    case DATA:
        state=MESI_CACHE_M;
        send_DATA_to_proc(request->addr);
        break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: SM state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_E (Mreq *request)
{
    switch (request->msg) {
    case GETS:
        set_shared_line();
        state = MESI_CACHE_S;
        break;
    case GETM:
        set_shared_line();
        state = MESI_CACHE_I;
        break;
    case DATA:
        send_DATA_to_proc(request->addr);
        break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Snoop Client: E state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_EM (Mreq *request)
{
    switch (request->msg) {
    case GETS:
        break;
    case GETM:
        break;
    case DATA:
        break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: EM state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_M (Mreq *request)
{
    switch (request->msg) {
    case GETS:
        set_shared_line();
        state = MESI_CACHE_S;
        send_DATA_on_bus(request->addr,request->src_mid);
        break;
    case GETM:
        set_shared_line();
        state = MESI_CACHE_I;
        send_DATA_on_bus(request->addr,request->src_mid);
        break;
    case DATA:
        fatal_error ("Client: M state shouldn't see this message\n");
        break;
    default:
        request->print_msg (my_table->moduleID, "ERROR");
        fatal_error ("Client: M state shouldn't see this message\n");
    }
}
