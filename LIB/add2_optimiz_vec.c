#include "flexaid.h"
#include "boinc.h"

#define N_NEW_PAR  5

/***************************************************************************** 
 * SUBROUTINE add2_optimiz_vec builds the vector with the values of the ic's
 * that are going to be optimized for one or more residues/ligands.
 *****************************************************************************/
void add2_optimiz_vec(FA_Global* FA,atom* atoms,resid* residue,int val[], char chain, const char* extras){
	int i;
	int at;
	//int rot;
	
	// val[0]=residue number
	// val[1]=0 -> optimization of global position; 
	// val[1]=n -> optimization of dihedral bond number n
		
	i=1;
	while(residue[i].number != val[0] || residue[i].chn != chain){
		i++;
		if(i == FA->res_cnt) break;
	}
	at=i;
  
	buildic(FA,atoms,residue,at);

	if(strcmp(extras,"SC") == 0){

		for(i=0;i<FA->nflxsc;i++){
            
			if(residue[FA->flex_res[i].inum].trot > 0){
				
				//printf("new par flex sc: %s %c %d\n", residue[FA->flex_res[i].inum].name,residue[FA->flex_res[i].inum].chn, residue[FA->flex_res[i].inum].number);
				//printf("npar: %d - MIN_PAR: %d\n", FA->npar,FA->MIN_PAR);
                
				if(FA->npar==FA->MIN_PAR){ realloc_par(FA,&FA->MIN_PAR); }
				
				FA->map_par[FA->npar].atm = residue[FA->flex_res[i].inum].fatm[0];
				FA->map_par[FA->npar].typ = 4;
				FA->opt_par[FA->npar] = 0.0;
				FA->del_opt_par[FA->npar] = FA->delta_index;
				FA->min_opt_par[FA->npar] = 0.0;
				FA->max_opt_par[FA->npar] = (double)residue[FA->flex_res[i].inum].trot;
				FA->map_opt_par[FA->npar] = 1;

				atoms[residue[FA->flex_res[i].inum].fatm[0]].par = &FA->map_par[FA->npar];
				
				printf("npar=%d map_par[%d].typ=%d map_par[%d].atm=%d opt_par[%d]=%f\n",FA->npar,	   
				       FA->npar,FA->map_par[FA->npar].typ,
				       FA->npar,atoms[FA->map_par[FA->npar].atm].number,
				       FA->npar,FA->opt_par[FA->npar]);
				//	  printf("min=%f max=%f del=%f\n",FA->min_opt_par[FA->npar],FA->max_opt_par[FA->npar],FA->del_opt_par[FA->npar]);

				if(FA->map_par_sidechain_first == NULL){
					FA->map_par_sidechain_first = &FA->map_par[FA->npar];
					FA->map_par_sidechain_first_index = FA->npar;
				}
				FA->map_par_sidechain_last = &FA->map_par[FA->npar];
				
				FA->npar++;
                
			}
            
		}
        
	}else if(strcmp(extras,"NM") == 0){
                
		if (FA->normal_modes > 0){
            
			if(FA->npar==FA->MIN_PAR){ realloc_par(FA,&FA->MIN_PAR); }
            
			FA->map_par[FA->npar].typ = 3;
			FA->opt_par[FA->npar] = 0.0;
			FA->del_opt_par[FA->npar] = FA->delta_index;
			FA->min_opt_par[FA->npar] = FA->normalindex_min;
			FA->max_opt_par[FA->npar] = FA->normalindex_max;
			FA->map_opt_par[FA->npar] = 1;
            
			printf("npar=%d map_par[%d].typ=%d map_par[%d].atm=%d opt_par[%d]=%f\n",FA->npar,	   
			       FA->npar,FA->map_par[FA->npar].typ,
			       FA->npar,atoms[FA->map_par[FA->npar].atm].number,
			       FA->npar,FA->opt_par[FA->npar]);
			//      printf("min=%f max=%f del=%f\n",FA->min_opt_par[FA->npar],FA->max_opt_par[FA->npar],FA->del_opt_par[FA->npar]);
            
			FA->npar++;
		}
        
	}else if(val[1] == -1){    // (1 degree of freedom of translation)

		if(FA->npar==FA->MIN_PAR){ realloc_par(FA,&FA->MIN_PAR); }

		FA->map_par[FA->npar].typ = -1; //anchor point in space (3 degrees of freedom of translation)
		FA->map_par[FA->npar].atm = residue[at].gpa[0];
		FA->opt_par[FA->npar] = 0.0; //sets default position to sphere index 0
		FA->del_opt_par[FA->npar] = FA->delta_index;
		FA->min_opt_par[FA->npar] = FA->index_min;
		FA->max_opt_par[FA->npar] = FA->index_max;
		FA->map_opt_par[FA->npar] = 1;
        
		atoms[residue[at].gpa[0]].par = &FA->map_par[FA->npar];
		
		printf("npar=%d map_par[%d].typ=%d map_par[%d].atm=%d opt_par[%d]=%f\n",FA->npar,
		       FA->npar,FA->map_par[FA->npar].typ,
		       FA->npar,atoms[FA->map_par[FA->npar].atm].number,
		       FA->npar,FA->opt_par[FA->npar]);
		//PAUSE;
        
		FA->npar++;
		
		FA->translational = 1;
		
	}else if(val[1] == 0){          // (3 degrees of freedom of rotation)
    
		if(FA->npar==FA->MIN_PAR){ realloc_par(FA,&FA->MIN_PAR); }
    
		for (i=1;i<=3;i++) {
			FA->map_par[FA->npar].bnd=0;

			if (i==1) {
				FA->map_par[FA->npar].typ = 1;  //ang
				FA->map_par[FA->npar].atm = residue[at].gpa[1];
				FA->opt_par[FA->npar] = atoms[FA->map_par[FA->npar].atm].ang;
				FA->del_opt_par[FA->npar] = FA->delta_angle;
				FA->min_opt_par[FA->npar] = -180.0;
				FA->max_opt_par[FA->npar] = 180.0;
				FA->map_opt_par[FA->npar] = 0;

				atoms[residue[at].gpa[1]].par = &FA->map_par[FA->npar];
				
			}else if (i==2) {
				FA->map_par[FA->npar].typ = 2;  //dih
				FA->map_par[FA->npar].bnd = -1;
				FA->map_par[FA->npar].atm=residue[at].gpa[1];
				FA->opt_par[FA->npar]=atoms[FA->map_par[FA->npar].atm].dih;
				FA->del_opt_par[FA->npar] = FA->delta_dihedral;
				FA->min_opt_par[FA->npar] = -180.0;
				FA->max_opt_par[FA->npar] = 180.0;
				FA->map_opt_par[FA->npar] = 0;
				
				atoms[residue[at].gpa[1]].par = &FA->map_par[FA->npar];

			}else if (i==3) {
				FA->map_par[FA->npar].typ = 2;
				FA->map_par[FA->npar].atm=residue[at].gpa[2];
				FA->opt_par[FA->npar]=atoms[FA->map_par[FA->npar].atm].dih;
				FA->del_opt_par[FA->npar] = FA->delta_dihedral;
				FA->min_opt_par[FA->npar] = -180.0;
				FA->max_opt_par[FA->npar] = 180.0;
				FA->map_opt_par[FA->npar] = 0;
				
				atoms[residue[at].gpa[2]].par = &FA->map_par[FA->npar];

			}

			printf("npar=%d map_par[%d].typ=%d map_par[%d].atm=%d opt_par[%d]=%f\n",FA->npar,
			       FA->npar,FA->map_par[FA->npar].typ,
			       FA->npar,atoms[FA->map_par[FA->npar].atm].number,
			       FA->npar,FA->opt_par[FA->npar]);
			//PAUSE;
            
			FA->npar++;
		}
  
	}else if(val[1] > 0){   // val[1] > 0 (dihedrals rotation)
        
		if(FA->npar==FA->MIN_PAR){ realloc_par(FA,&FA->MIN_PAR); }
		
		/* dihedral angle optimization */
		//FA->intramolecular = 1;
		FA->map_par[FA->npar].typ = 2;
		FA->map_par[FA->npar].bnd = val[1];
		FA->map_par[FA->npar].atm = residue[at].bond[val[1]];
		FA->opt_par[FA->npar] = atoms[FA->map_par[FA->npar].atm].dih;
		FA->del_opt_par[FA->npar] = FA->delta_flexible;
		FA->min_opt_par[FA->npar] = -180.0;
		FA->max_opt_par[FA->npar] = 180.0;
		FA->map_opt_par[FA->npar] = 0;

		atoms[residue[at].bond[val[1]]].par = &FA->map_par[FA->npar];
		
		FA->nflexbonds++;

		printf("npar=%d map_par[%d].typ=%d map_par[%d].atm=%d opt_par[%d]=%f\n",FA->npar,
		       FA->npar,FA->map_par[FA->npar].typ,
		       FA->npar,atoms[FA->map_par[FA->npar].atm].number,
		       FA->npar,FA->opt_par[FA->npar]);
		//PAUSE;

		if(FA->map_par_flexbond_first == NULL){
			FA->map_par_flexbond_first = &FA->map_par[FA->npar];
			FA->map_par_flexbond_first_index = FA->npar;
		}
		FA->map_par_flexbond_last = &FA->map_par[FA->npar];
        
		FA->npar++;
	}
	
	return;
}


void realloc_par(FA_Global* FA, int* MIN_PAR){
	
	*MIN_PAR += N_NEW_PAR;

	FA->map_par = (optmap*)realloc(FA->map_par,(*MIN_PAR)*sizeof(optmap));
	FA->opt_par = (double*)realloc(FA->opt_par,(*MIN_PAR)*sizeof(double));
	FA->del_opt_par = (double*)realloc(FA->del_opt_par,(*MIN_PAR)*sizeof(double));
	FA->min_opt_par = (double*)realloc(FA->min_opt_par,(*MIN_PAR)*sizeof(double));
	FA->max_opt_par = (double*)realloc(FA->max_opt_par,(*MIN_PAR)*sizeof(double));
	FA->map_opt_par = (int*)realloc(FA->map_opt_par,(*MIN_PAR)*sizeof(int));
	
	if(!FA->map_par || !FA->opt_par ||
	   !FA->del_opt_par || !FA->min_opt_par || 
	   !FA->max_opt_par || !FA->map_opt_par){
		
		fprintf(stderr,"ERROR: memory allocation error for opt_par\n");
		Terminate(2);
	}
	
	memset(&FA->map_par[(*MIN_PAR)-N_NEW_PAR],0,N_NEW_PAR*sizeof(optmap));
	memset(&FA->opt_par[(*MIN_PAR)-N_NEW_PAR],0,N_NEW_PAR*sizeof(double));
	memset(&FA->del_opt_par[(*MIN_PAR)-N_NEW_PAR],0,N_NEW_PAR*sizeof(double));
	memset(&FA->min_opt_par[(*MIN_PAR)-N_NEW_PAR],0,N_NEW_PAR*sizeof(double));
	memset(&FA->max_opt_par[(*MIN_PAR)-N_NEW_PAR],0,N_NEW_PAR*sizeof(double));
	memset(&FA->map_opt_par[(*MIN_PAR)-N_NEW_PAR],0,N_NEW_PAR*sizeof(int));
      	
}
