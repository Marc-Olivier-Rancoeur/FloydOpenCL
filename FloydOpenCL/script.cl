__kernel void floyd(const unsigned int taille, __global unsigned int* matrice, const unsigned int k){
	int i = get_global_id(0);
	int j = get_global_id(1);
	if((matrice[(taille * i) + k] + matrice[(taille * k) + j]) < matrice[(taille * i) + j]){
		matrice[(taille * i) + j] = matrice[(taille * i) + k] + matrice[(taille * k) + j];
	}
}