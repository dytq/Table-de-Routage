#include <stdio.h>

//~ local
#include "const.h"
#include "graph.h"
#include "moteur_graphique.h"
#include "routage.h"

void afficher_voisin(graphe* G, flame_obj_t * fo, cercle_t * c,int sommet, int deb, int fin)
{
	int j;

	for(j = deb; j < fin; j++)
	{
		if(G->list[sommet][j] != -1)
		{
			afficher_connexion(fo,c,sommet,j,GRIS);
		}
	}
}


void init_affichage_tier(cercle_t * c, int debut,int fin, int tier,int * x,int * y)
{
	int i;
	
	for( i = debut ; i < fin ; i ++)
	{		
		c[i].rad = TAILLE_CERCLE;
		if(tier == tier1)
		{
			colorer_cercle(&c[i],ROUGE);
			c[i].pos_x = *x * ( 3 * TAILLE_CERCLE);
			c[i].pos_y = *y * ( 3 * TAILLE_CERCLE);
			
		}
		if(tier == tier2)
		{
			colorer_cercle(&c[i],BLEU);
			c[i].pos_x = *x * ( 3 * TAILLE_CERCLE);
			c[i].pos_y = *y * ( 3 * TAILLE_CERCLE);
		}
		if(tier == tier3)
		{
			colorer_cercle(&c[i],VERT);
			c[i].pos_x = *x * ( 3 * TAILLE_CERCLE);
			c[i].pos_y = *y * ( 3 * TAILLE_CERCLE);
		}
		*x+=1;
		if(*x > 10)
		{
			*y += 1;
			*x = 1;
		}
	}
}

void initialisation_objets_graphique(graphe *G,flame_obj_t * fo,cercle_t * c) {
	int noeud;
	
	int x = 1;
	int y = 1;
	init_affichage_tier(c,debTier1,finTier1,tier1,&x,&y);
	init_affichage_tier(c,debTier2,finTier2,tier2,&x,&y);
	init_affichage_tier(c,debTier3,finTier3,tier3,&x,&y);
	
	for (noeud = 0; noeud < TAILLE_GRAPHE; noeud++)
	{
		afficher_cercle(fo, &c[noeud]);
		
		// Ca sert a rien
		
		//~ afficher_voisin(G, fo, c, noeud, debTier1, finTier1);
		//~ afficher_voisin(G, fo, c, noeud, debTier2, finTier2);
		//~ afficher_voisin(G, fo, c, noeud, debTier3, finTier3);
	}	
}
int trouve_id(int x,int y)
{
	x -= (2 * TAILLE_CERCLE);
	y -= (2 * TAILLE_CERCLE);
	return ((y / (3 * TAILLE_CERCLE))*10) + (x / (3*TAILLE_CERCLE));
}

void affiche_chemin (flame_obj_t * fo,routage* R, cercle_t * c,int deb, int fin,enum couleur coul) {
	int voisin[TAILLE_GRAPHE] = {-1};
	
	int i = 0, suiv = R->succ[deb][fin];
	voisin[i] = deb;
	
	for(i = 1; suiv != fin && i < TAILLE_GRAPHE; i++)
	{
		deb = R->succ[deb][fin];
		voisin[i] = deb;
		suiv = R->succ[deb][fin];
	}
	
	if(suiv == fin)
	{
		voisin[i] = fin;
		
		int d, e, j;
		for(j = 0 ; j < i; j++)
		{
			d = voisin[j]; e = voisin[j+1];
			afficher_connexion(fo, c, d, e, coul);
		}
	}
	else
	{
		printf("error\n");
	}
}


void interaction_joueur(graphe * G,flame_obj_t * fo,routage * R,cercle_t * c)
{
	XEvent event;
	int cmp = 0;
	int id_1 = 0;
	int id_2 = 0;
	int save_id_1 = 0;
	int save_id_2 = 0;

	int click_x, click_y;
	
	while (1)
    {
		if (XPending(fo->display) > 0)
		{
		  XNextEvent(fo->display, &event);
		  if(recupere_clavier(event) == 'q') { break; }
		  if (event.type == ButtonPress)
		  {
			  
				click_x = event.xkey.x;
				click_y = event.xkey.y;	
				
				colorer_cercle(&c[trouve_id(click_x,click_y)],JAUNE);
				afficher_cercle(fo,&c[trouve_id(click_x,click_y)]);
				
				if(cmp == 0)
				{
					save_id_1 = id_1;
					id_1 = trouve_id(click_x,click_y);
				}
				else
				{
					save_id_2 = id_2;
					id_2 = trouve_id(click_x,click_y);
				}
				cmp ++;
		  }
	  }
	  else
	  {
		if(cmp == 2)
		{
			cmp = 0;
			affiche_chemin ( fo, R, c, save_id_1, save_id_2, NOIR);
			initialisation_objets_graphique ( G, fo, c);
			
			afficher_chemin (R, id_1, id_2);
			affiche_chemin (fo, R, c, id_1, id_2, JAUNE);
			
			colorer_cercle( &c[id_1], JAUNE);
			colorer_cercle( &c[id_2], JAUNE);
			
			afficher_cercle(fo, &c[id_1]);
			afficher_cercle(fo, &c[id_2]);
		}  
		else
		{
			usleep(800000);
			// Ca fait lager ca
			
			//~ affiche_chemin(fo,R,c,id_1,id_2,NOIR);
			//~ initialisation_objets_graphique(G,fo,c);
		}
	  }
  }
}

void gestion_fenetre_graphique(graphe* G, routage *R)
{
	// > Initialisation du canvas:
	flame_obj_t * fo = init_canvas();
	
	cercle_t c[TAILLE_GRAPHE];
	
	initialisation_objets_graphique(G,fo,c);
	
	interaction_joueur(G,fo,R,c);
	flame_close(fo);
}
