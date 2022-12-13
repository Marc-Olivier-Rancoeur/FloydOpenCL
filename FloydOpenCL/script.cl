__kernel void floyd(int taille, __global unsigned int* matrice){
	int j = get_global_id(0);
	int k = get_global_id(1);
	for(unsigned int l = 0 ; l < taille ; l++){
	for(unsigned int i = 0 ; i < taille ; i++){
		if((matrice[(taille * j) + i] + matrice[(taille * i) + k]) < matrice[(taille * j) + k]){
			matrice[(taille * j) + k] = matrice[(taille * j) + i] + matrice[(taille * i) + k];
		}
	}
	}
}