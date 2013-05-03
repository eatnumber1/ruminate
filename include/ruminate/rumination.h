#ifndef _RUMINATION_H_
#define _RUMINATION_H_

struct Rumination;
typedef struct Rumination Rumination;

void rumination_delete( Rumination **rum );
Rumination *rumination_new( GError **err );

#endif
