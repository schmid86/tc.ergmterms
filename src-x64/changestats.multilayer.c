/* This is the C side of duplexdyad. */
#include "changestats.users.h"

D_CHANGESTAT_FN(d_duplexdyad){
	/* Declaring variables */
	int i, j, la, lb, type[N_CHANGE_STATS], layer_mem[N_NODES]; 
	int changes[5], edge_status[4];
	Vertex tail, head, t_neighbour, h_neighbour;
	Edge e1, e2;
	
	/* Breaking the INPUT_PARAM into the layers used, all the types to compute (1 to 5), and the membership of nodes (one of the two layers)*/
	/* The INPUT_PARAM array should have first and second elements as layers; up to five elements as statistics; and the rest as layer membership*/
	la = INPUT_PARAM[0]; lb = INPUT_PARAM[1];
	for(i = 0; i < N_CHANGE_STATS; i++){
		type[i] = INPUT_PARAM[2 + i];
	}
	for(i = 0; i < N_NODES; i++){
		layer_mem[i] = INPUT_PARAM[2 + N_CHANGE_STATS + i];
	}
	
	ZERO_ALL_CHANGESTATS(i);
	FOR_EACH_TOGGLE(i){
		tail = TAIL(i); head = HEAD(i);
		for(j = 0; j < 5; j++){changes[j] = 0;}
		for(j = 0; j < 4; j++){edge_status[j] = 0;}
		/* Check incident layers of the edge to determine edge type */
		if(layer_mem[tail - 1] == layer_mem[head - 1]){ /* Are they intralayer or interlayer edges?*/
			if(layer_mem[tail - 1] == la){ /* Edge on layer a */
				/* Determine the status of the tail-head dyad */
				edge_status[0] = IS_OUTEDGE(tail, head);
				edge_status[1] = IS_INEDGE(tail, head);
				
				/* Begin looping through interlayer outedges of tail*/
				STEP_THROUGH_OUTEDGES(tail, e1, t_neighbour){
					if(layer_mem[t_neighbour - 1] == lb){ /* Make sure it is an interlayer edge */
						/* Begin looping through interlayer outedge of head*/
						STEP_THROUGH_OUTEDGES(head, e2, h_neighbour){
							if(layer_mem[h_neighbour - 1] == lb && t_neighbour != h_neighbour){ /* Make sure it is an interlayer edge AND that is is not the same as the tail neighbour */
								/* Determine the status of the t_neighbour-h_neighbour dyad */
								edge_status[2] = IS_OUTEDGE(t_neighbour, h_neighbour);
								edge_status[3] = IS_INEDGE(t_neighbour, h_neighbour);
								/* Change statistic (turning the t-> edge from 0 to 1 */
								if(edge_status[1] == 0 && edge_status[2] == 1 && edge_status[3] == 0){changes[0] += 1;} /*_010*/
								if(edge_status[1] == 0 && edge_status[2] == 0 && edge_status[3] == 1){changes[1] += 1;} /*_001*/
								if(edge_status[1] == 0 && edge_status[2] == 1 && edge_status[3] == 1){changes[0] += 1; changes[1] += 1; changes[3] += 1;} /*_011*/
								if(edge_status[1] == 1 && edge_status[2] == 1 && edge_status[3] == 0){changes[0] += 1; changes[2] += 1;} /*_110*/
								if(edge_status[1] == 1 && edge_status[2] == 0 && edge_status[3] == 1){changes[1] += 1; changes[2] += 1;} /*_101*/
								if(edge_status[1] == 1 && edge_status[2] == 1 && edge_status[3] == 1){changes[0] += 1; changes[1] += 1; changes[2] += 2; changes[3] += 1; changes[4] += 1;} /*_111*/
							}
						}
					}
				}
			}else{ /* Edge on layer b */
				/* Determine status of the tail-head dyad */
				edge_status[2] = IS_OUTEDGE(tail, head);
				edge_status[3] = IS_INEDGE(tail, head);
        
				/* Begin loop based on cross layer edge from tail*/
				STEP_THROUGH_INEDGES(tail, e1, t_neighbour){
					if(layer_mem[t_neighbour - 1] == la){
						/* Begin loop based on cross layer edge from head*/
						STEP_THROUGH_INEDGES(head, e2, h_neighbour){
							if(layer_mem[h_neighbour - 1] == la && t_neighbour != h_neighbour){
								/* Determine the status of the dyad on layer b}}.*/
								edge_status[0] = IS_OUTEDGE(t_neighbour, h_neighbour);
								edge_status[1] = IS_INEDGE(t_neighbour, h_neighbour);
								/* Define the observed structure (based on everything but t->h) */
								if(edge_status[0] == 1 && edge_status[1] == 0 && edge_status[3] == 0){changes[0] += 1;} /*10_0*/
								if(edge_status[0] == 0 && edge_status[1] == 1 && edge_status[3] == 0){changes[1] += 1;} /*01_0*/
								if(edge_status[0] == 1 && edge_status[1] == 1 && edge_status[3] == 0){changes[0] += 1; changes[1] += 1; changes[2] += 1;} /*11_0*/
								if(edge_status[0] == 1 && edge_status[1] == 0 && edge_status[3] == 1){changes[0] += 1; changes[3] += 1;} /*10_1*/
								if(edge_status[0] == 0 && edge_status[1] == 1 && edge_status[3] == 1){changes[1] += 1; changes[3] += 1;} /*01_1*/
								if(edge_status[0] == 1 && edge_status[1] == 1 && edge_status[3] == 1){changes[0] += 1; changes[1] += 1; changes[2] += 1; changes[3] += 2; changes[4] += 1;} /*11_1*/
							}
						}
					}
				}
			}
		}else{  /* Edge crosses layers */
			if(layer_mem[tail - 1] == la){
				for(t_neighbour = 1; t_neighbour <= N_NODES; t_neighbour++){ /* Loop through tail neighbours*/
					for(h_neighbour = 1; h_neighbour <= N_NODES; h_neighbour++){ /* Loop through head neighbours*/
						if( (IS_OUTEDGE(tail, t_neighbour) || IS_INEDGE(tail, t_neighbour)) && /* Make sure the four nodes are part of a duplex dyad*/
							(IS_OUTEDGE(head, h_neighbour) || IS_INEDGE(head, h_neighbour)) && 
							(layer_mem[t_neighbour - 1] == la) && /* tail->t_neighbour must be an intralayer edge */
							(layer_mem[h_neighbour - 1] == lb) && /* head->h_neighbour must be an intralayer edge */
							(tail != t_neighbour) && (head != h_neighbour) && 
							(IS_OUTEDGE(t_neighbour, h_neighbour))){ /* t_neighbour and h_neighbour must be connected by an interlayer edge */
							/* Determining the status of ;the tail->t_neighbour dyad and the head->h_neighbour dyad*/							
							edge_status[0] = IS_OUTEDGE(tail, t_neighbour);
							edge_status[1] = IS_INEDGE(tail, t_neighbour);
							edge_status[2] = IS_OUTEDGE(head, h_neighbour);
							edge_status[3] = IS_INEDGE(head, h_neighbour);
							
							/* Define the observed structure (based on everything but t->h) */
							if(edge_status[0]== 1 && edge_status[1]== 1 && edge_status[2]== 1 && edge_status[3]== 1){changes[0] += 2; changes[1] += 2; changes[2] += 2; changes[3] += 2; changes[4] += 1;} /*I*/
							if(edge_status[0]== 0 && edge_status[1]== 1 && edge_status[2]== 1 && edge_status[3]== 1){changes[0] += 1; changes[1] += 1; changes[3] += 1;} /*H*/
							if(edge_status[0]== 1 && edge_status[1]== 0 && edge_status[2]== 1 && edge_status[3]== 1){changes[0] += 1; changes[1] += 1; changes[3] += 1;} /*H*/
							if(edge_status[0]== 1 && edge_status[1]== 1 && edge_status[2]== 1 && edge_status[3]== 0){changes[0] += 1; changes[1] += 1; changes[2] += 1;} /*G*/
							if(edge_status[0]== 1 && edge_status[1]== 1 && edge_status[2]== 0 && edge_status[3]== 1){changes[0] += 1; changes[1] += 1; changes[2] += 1;} /*G*/
							if(edge_status[0]== 1 && edge_status[1]== 0 && edge_status[2]== 0 && edge_status[3]== 1){changes[1] += 1;} /*F*/
							if(edge_status[0]== 0 && edge_status[1]== 1 && edge_status[2]== 1 && edge_status[3]== 0){changes[1] += 1;} /*F*/
							if(edge_status[0]== 1 && edge_status[1]== 0 && edge_status[2]== 1 && edge_status[3]== 0){changes[0] += 1;} /*E*/
							if(edge_status[0]== 0 && edge_status[1]== 1 && edge_status[2]== 0 && edge_status[3]== 1){changes[0] += 1;} /*E*/
							
						}
					}
				}
			}
		}
		/* Changing CHANGE_STAT */
		for(j = 0; j < N_CHANGE_STATS; j++){
			CHANGE_STAT[j] += IS_OUTEDGE(tail, head) ? -changes[type[j] - 1] : changes[type[j] - 1];		  
		}
    		// The current version of MPNet (v1.04) double counts certain cross-layer statistics; uncomment the following loop to reproduce exactly that.
		//for(j = 0; j < N_CHANGE_STATS; j++){
		//	CHANGE_STAT[j] *= 2;
		//}
		TOGGLE_IF_MORE_TO_COME(i);
	}
	UNDO_PREVIOUS_TOGGLES(i);
}

D_CHANGESTAT_FN(d_gwtdsp_layer) {
	Edge e, f;
	int i, echange, ochange, L2tu, L2uh, l, layer_mem[N_NODES];
	Vertex tail, head, u, v;
	double alpha, oneexpa, cumchange;

	CHANGE_STAT[0] = 0.0;
	alpha = INPUT_PARAM[0];
	oneexpa = 1.0-exp(-alpha);
	l = INPUT_PARAM[1];
	for(i = 0; i < N_NODES; i++){
		layer_mem[i] = INPUT_PARAM[2 + i];
	}

	/* *** don't forget tail -> head */    
	FOR_EACH_TOGGLE(i){
		tail=TAIL(i); head=HEAD(i);
		if(layer_mem[tail - 1] == l && layer_mem[head - 1] == l){
			cumchange=0.0;
			ochange = -IS_OUTEDGE(tail,head);
			echange = 2*ochange + 1;
			/* step through outedges of head */
			for(e = MIN_OUTEDGE(head); (u=OUTVAL(e))!=0; e=NEXT_OUTEDGE(e)) {
				if (u != tail && layer_mem[u - 1] == l){
					L2tu=ochange; /* L2tu will be # shrd prtnrs of (tail,u) not incl. head */
					/* step through inedges of u, incl. (head,u) itself */
					for(f = MIN_INEDGE(u); (v=INVAL(f))!=0; f=NEXT_INEDGE(f)) {
						if(IS_OUTEDGE(tail,v) && layer_mem[v - 1] == l) L2tu++;
					}
					cumchange += pow(oneexpa,(double)L2tu); /* sign corrected below */
				}
			}
			/* step through inedges of tail */
			for(e = MIN_INEDGE(tail); (u=INVAL(e))!=0; e=NEXT_INEDGE(e)) {
				if (u != head && layer_mem[u - 1] == l){
					L2uh=ochange; /* L2uh will be # shrd prtnrs of (u,head) not incl. tail */
					/* step through outedges of u , incl. (u,tail) itself */
					for(f = MIN_OUTEDGE(u);(v=OUTVAL(f))!=0; f=NEXT_OUTEDGE(f)){
						if(IS_OUTEDGE(v,head) && layer_mem[v - 1] == l) L2uh++;
					}
					cumchange += pow(oneexpa,(double)L2uh); /* sign corrected below */
				}
			}
			CHANGE_STAT[0] += echange * cumchange;
		}
		TOGGLE_IF_MORE_TO_COME(i);
	}
	UNDO_PREVIOUS_TOGGLES(i);
}

D_CHANGESTAT_FN(d_gwtesp_layer) { 
	Edge e, f;
	int i, echange, ochange;
	int L2th, L2tu, L2uh, l, layer_mem[N_NODES];
	Vertex tail, head, u, v;
	double alpha, oneexpa, cumchange;
  
	CHANGE_STAT[0] = 0.0;
	alpha = INPUT_PARAM[0];
	oneexpa = 1.0-exp(-alpha);
	l = INPUT_PARAM[1];
	for(i = 0; i < N_NODES; i++){
		layer_mem[i] = INPUT_PARAM[2 + i];
	}
	
	/* *** don't forget tail -> head */    
	FOR_EACH_TOGGLE(i){
		tail = TAIL(i); head = HEAD(i);
		if(layer_mem[tail - 1] == l && layer_mem[head - 1] == l){
			cumchange=0.0;
			L2th=0;
			ochange = IS_OUTEDGE(tail, head) ? -1 : 0;
			echange = 2*ochange + 1;
			/* step through outedges of head  */
			STEP_THROUGH_OUTEDGES(head, e, u){
				if (IS_OUTEDGE(tail, u) && layer_mem[u - 1] == l){
					L2tu=ochange;
					/* step through inedges of u */
					STEP_THROUGH_INEDGES(u, f, v){
						if(IS_OUTEDGE(tail, v) && layer_mem[v - 1] == l) L2tu++;
					}
					cumchange += pow(oneexpa,(double)L2tu);
				}
			}
			
			/* step through inedges of head */
			STEP_THROUGH_INEDGES(head, e, u){
				if (IS_OUTEDGE(tail, u)){
					L2th++;
				}
				if (IS_OUTEDGE(u, tail) && layer_mem[u - 1] == l){
					L2uh=ochange;
					/* step through outedges of u */
					STEP_THROUGH_OUTEDGES(u, f, v){
						if(IS_OUTEDGE(v, head) && layer_mem[v - 1] == l) L2uh++;
					}
					cumchange += pow(oneexpa,(double)L2uh) ;
				}
			}
			
			if(alpha < 100.0){
				cumchange += exp(alpha)*(1.0-pow(oneexpa,(double)L2th)) ;
			}else{
				cumchange += (double)L2th;
			}
			cumchange  = echange*cumchange;
			(CHANGE_STAT[0]) += cumchange;
		}
		TOGGLE_IF_MORE_TO_COME(i);
	}
	UNDO_PREVIOUS_TOGGLES(i);
}

