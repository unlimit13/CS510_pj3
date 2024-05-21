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
    const char *block_states[10] = {"X","I","IS","IE","IM","S","SM","E","EM","M"};
    fprintf (stderr, "MESI_protocol - state: %s\n", block_states[state]);
}

void MESI_protocol::process_cache_request (Mreq *request)
{
	switch (state) {
    case MESI_CACHE_I: do_cache_I(request); break;
    case MESI_CACHE_IS: do_cache_IS(request); break;
    case MESI_CACHE_IE: do_cache_IE(request); break;
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
    case MESI_CACHE_IS: do_snoop_IS(request); break;
    case MESI_CACHE_IE: do_snoop_IE(request); break;
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
            send_GETS(request->addr);
            if(get_shared_line()){
                state = MESI_CACHE_IS;
            }
            else{
                state = MESI_CACHE_IE;
            }
            Sim->cache_misses++;
            break;

        case STORE:
            send_GETM(request->addr);
            state = MESI_CACHE_IM;
            Sim->cache_misses++;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: I state shouldn't see this message\n");
    }
}
inline void MESI_protocol::do_cache_IS (Mreq *request)
{

}
inline void MESI_protocol::do_cache_IE (Mreq *request)
{

}
inline void MESI_protocol::do_cache_IM (Mreq *request)
{

}

inline void MESI_protocol::do_cache_S (Mreq *request)
{

}
inline void MESI_protocol::do_cache_SM (Mreq *request)
{

}
inline void MESI_protocol::do_cache_E (Mreq *request)
{

}
inline void MESI_protocol::do_cache_EM (Mreq *request)
{

}
inline void MESI_protocol::do_cache_M (Mreq *request)
{

}

inline void MESI_protocol::do_snoop_I (Mreq *request)
{

}
inline void MESI_protocol::do_snoop_IS (Mreq *request)
{

}
inline void MESI_protocol::do_snoop_IE (Mreq *request)
{

}
inline void MESI_protocol::do_snoop_IM (Mreq *request)
{

}

inline void MESI_protocol::do_snoop_S (Mreq *request)
{

}
inline void MESI_protocol::do_snoop_SM (Mreq *request)
{

}
inline void MESI_protocol::do_snoop_E (Mreq *request)
{

}

inline void MESI_protocol::do_snoop_EM (Mreq *request)
{

}
inline void MESI_protocol::do_snoop_M (Mreq *request)
{

}

