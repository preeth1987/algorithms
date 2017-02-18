/*
Distance Vector Routing in this program is implemented using Bellman Ford Algorithm:-
*/
#include<stdio.h>
#include<stdlib.h>

struct path
{
	unsigned int hop_count;
	unsigned int *hops;
};
struct node
{
    unsigned int *dist;
    unsigned int *from;
	struct path *path_info;
};

cleanup(unsigned int **ptr, int ncount)
{
	int i = 0;
	for (i=0;i<ncount;++i)
    {
		free((void*)ptr[i]);
	}
	free((void*)ptr);
}

/*Reads file, allocates memory for matrix, fills samed in matrix*/
/*Returns number of nodes in topolog*/
unsigned int **
parseMatrix(int *node_count)
{
	char file_name[25];
	FILE *fp;
	char line[100];
	char c;
	int	 ncount = 0,i,j;
	unsigned int **matrix = NULL;

	printf("Enter the name of cost matrix file\n");
	
	gets(file_name);
    
    fp = fopen(file_name,"r"); // read mode
    
    if( fp == NULL )
    {   
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

	fscanf(fp,"%[^\n]",line);
    
	for(i=0;line[i]!='\0';i++){
        if(line[i]==' '){
            ncount++;
            while(line[i]==' ')
                i++;
        }
    }
	fseek(fp, SEEK_SET, 0);

	printf("nodes in topology: %d\n", ncount);
    matrix = (unsigned int**)calloc(ncount, sizeof(unsigned int*));
    if(matrix)
    {
        for (i=0;i<ncount;++i)
        {
            matrix[i] = (int*)calloc(ncount, sizeof(unsigned int));
        }

    } else {
        printf("calloc failed\n");
        return 0;
    }

    i = 0;
    j=0;
    do {
        if (j == ncount)
            break;
        c = fscanf(fp,"%s",line);
        matrix[j][i++] = atoi(line);
        if (i == ncount )
        {
            i = 0;
            j++;
        }
    } while (c != EOF);              /* repeat until EOF           */
    
	printf("Cost metrix is :\n");
	for (i=0;i<ncount;i++)
    {
        for (j=0;j<ncount;j++)
            printf("%d  ", matrix[i][j]);
        printf("\n");
    }
    fclose(fp);
	printf("\n");

	*node_count = ncount;
	return matrix;

}

int dv(unsigned int **cost_matrix, int node_count)
{
	int i,j,k, count;
	//struct node matrix[node_count][node_count];
	struct node *matrix = NULL;
	int iterations = 0;
	struct path *pathAttr = NULL;
	unsigned int to, from, via, dest;
	unsigned int length = 0;
	char c;

	matrix = (struct node*)calloc(node_count, sizeof(struct node));
	if (matrix)
	{
		for (i=0;i<node_count;i++) {
			matrix[i].from = (unsigned int*)calloc(node_count, sizeof(unsigned int) );
			matrix[i].dist = (unsigned int*)calloc(node_count, sizeof(unsigned int));
			for (j=0;j<node_count;j++)
			{
				matrix[i].from[j] = 0;
				matrix[i].dist[j] = 999;
			}
			/*Allocate memory for path*/
			if ((pathAttr = (struct path*)calloc(1, sizeof(struct path))) != NULL)
			{
				pathAttr->hops = (unsigned int*)calloc(node_count, sizeof(unsigned int));
				matrix[i].path_info = pathAttr;
			}
			else 
			{
				printf("Failed to allocate memory\n");
				return 0;
			}
		}
	} else {
		printf("calloc failed\n");
		return 0;
	}

	for (i=0;i<node_count;i++)
    {
        for (j=0;j<node_count;j++)
		{
			cost_matrix[i][i] = 0;
			matrix[i].dist[j] = cost_matrix[i][j];
			matrix[i].from[j] = j;
		}
        printf("\n");
    }


	printf("Running DV algorithm node_count: %d\n", node_count);

	do
        {
            count=0;
            for(i=0;i<node_count;i++)//We choose arbitary vertex k and we calculate the direct distance from the node i to k using the cost matrix
            //and add the distance from k to node j
            for(j=0;j<node_count;j++)
            for(k=0;k<node_count;k++)
                if(matrix[i].dist[j]>cost_matrix[i][k]+matrix[k].dist[j])
                {//We calculate the minimum distance
                    matrix[i].dist[j]=matrix[i].dist[k]+matrix[k].dist[j];
                    matrix[i].from[j]=k;
					matrix[i].path_info->hops[matrix[i].path_info->hop_count++] = k;
                    count++;
					iterations++;
                }
        }while(count!=0);

	printf("\nNumber of iterations: %d\n", iterations);
	for(i=0;i<node_count;i++)
	{
		//printf("\n\n For router %d\n",i+1);
		for(j=0;j<node_count;j++)
		{
			printf("%d	", matrix[i].dist[j]);
			//printf("\t\nnode %d via %d Distance %d ",j+1,matrix[i].from[j]+1,matrix[i].dist[j]);
		}
		printf("\n");
	}

	printf("\n\n");
	
	c = 'y';
	while ( (c == 'y')  )
	{
		length = 0;
		printf("Please enter to and from:\n");
		scanf("%d", &to);
		scanf("%d", &from);
		to--;
		from--;
		dest = to;
		//printf("hop count: %d\n", matrix[to].path_info->hop_count);
		//via = matrix[to].from[from];
		via = from;
		printf("distance: %d\n", matrix[to].dist[from]);

		printf("\nPath :\n");
		printf("%d -> ", (from+1));	
		/*destination is not directly connected with shortest path*/
		if (matrix[to].from[from] != from) {
			do {
				via = matrix[to].from[from];
				from = via;
				printf("%d -> ", (from + 1));
				length++;
			} while (matrix[via].from[via] != via);
		}
		printf("%d\n", (dest + 1));
		printf("Length: %d\n", length);
		printf("Another source-destination pair?\n");
		fflush(stdout);
		fflush(stdin);
		scanf(" %c", &c);
	} 
}

int main ()
{
	unsigned int **matrix = NULL;
	int	node_count = 0;

	matrix = parseMatrix(&node_count);
	if ((node_count <= 0) || !matrix)
	{
		printf("\nFailed to parse cost matrix file\n");
		return 0;
	}	

	dv(matrix, node_count);

	cleanup(matrix, node_count);

	return 0;
}
