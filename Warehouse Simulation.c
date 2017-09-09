#include <stdio.h>
#include<stdlib.h>
#define MAX_ITEM 10
#define MAX_CUSTOMER 100
#define BIN_DISTANCE 3.8
#define CORRIDOR_DISTANCE 6.4
#define DEFAULT 8

typedef struct {
	int bin_num;
	char corridor_num;
}item;
typedef struct {
	int customer_id;
    int item_num;
    int item_id;
    item items[MAX_ITEM];
}order;

int read_order(order all_orders[],int *total_items);
int calcualte_corridor(char corridor,char previous_corridor);
int count_picker(order all_orders[],int num_customer,order new_orders[]);
int combine_orders(order all_orders[],int each_customer,order new_orders[],
		int num_picker);
double count_distance(order all_orders[],int num_customer,int row,int column,
		double distance[]);
void items_in_order(order all_orders[],int num_customer);
void item_swap(order all_orders[],int each_customer,int each_item,
	char previous_corridor,char corridor,int previous_bin,int bin);
void assign_values(order all_orders[],int each_customer,int i,
	char *previous_corridor,char *corridor,int *previous_bin,int *bin);
void print_firstnlast(order all_orders[],int num_customer);

/* Main function */
int main(int argc, char *argv[]){
	order all_orders[MAX_CUSTOMER],new_orders[MAX_CUSTOMER];
	int total_items=0,num_customer,row,column,num_picker;
	double distance[MAX_CUSTOMER],total_distance,combined;

	/* STAGE 1 */
	num_customer = read_order(all_orders,&total_items);
	printf("Stage 1\n-------\n");
	printf("  orders: %3d\n  items : %3d\n",num_customer,total_items);
	print_firstnlast(all_orders,num_customer);

	/* STAGE 2 */
	items_in_order(all_orders,num_customer);
	printf("Stage 2\n-------\n");
	print_firstnlast(all_orders,num_customer);

	/* STAGE 3 */
	/* Read inputs from keyboard and store the values as row and column,
	that are defaulted to 8 */
	if (argc != 1){
		row = atoi(argv[1]);
		column = atoi(argv[2]);
	}else{
		row = DEFAULT;
		column = DEFAULT;
	}
	total_distance=count_distance(all_orders,num_customer,row,column,distance);
	printf("Stage 3\n-------\n");
	printf("  warehouse has %2d rows and %2d columns\n",row,column);
	printf("  customer %d, %2d items, pick distance: %4.1lf metres\n",
			all_orders[0].customer_id,all_orders[0].item_num,distance[0]);
	printf("  customer %d, %2d items, pick distance: %4.1f metres\n",
			all_orders[num_customer-1].customer_id,
			all_orders[num_customer-1].item_num,distance[num_customer-1]);
	printf("  average distance per order over %2d orders: %4.1f metres\n\n",
			num_customer,total_distance/num_customer);

	/* STAGE 4 */
	num_picker = count_picker(all_orders,num_customer,new_orders);
	items_in_order(new_orders,num_picker);
	/* combined is total distance all the pickers need to travel while in the
	case of combining possibly two orders */
	combined = count_distance(new_orders,num_picker,row,column,distance);
	printf("Stage 4\n-------\n  pickers required: %2d\n  average distance "
		"per order over %2d orders: %4.1f metres",num_picker,num_customer,
		combined/num_customer);
	return 0;
}

/* Printing function that is applied in stage 1 and 2 which prints out
information related to the order by the first and last customers */
void print_firstnlast(order all_orders[],int num_customer){
	int each_item;
	printf("  customer %d, %2d items, bins: ",
		all_orders[0].customer_id,all_orders[0].item_num);
	for (each_item=0;each_item<all_orders[0].item_num;each_item++){
		printf("%2d%c ",all_orders[0].items[each_item].bin_num,
			all_orders[0].items[each_item].corridor_num);
	}
	printf("\n  customer %d, %2d items, bins: ",
		all_orders[num_customer-1].customer_id,
		all_orders[num_customer-1].item_num);
	for (each_item=0;each_item<all_orders[num_customer-1].item_num;
		each_item++){
		printf("%2d%c ",all_orders[num_customer-1].items[each_item].bin_num,
			all_orders[num_customer-1].items[each_item].corridor_num);
	}
	printf("\n\n");
}

/* read_order reads data from input file customer by customer and store the
orders into an array, the function returns the number of customer which is
also the number of orders, number of items is passed as a pointer */
int read_order(order all_orders[],int *total_items){
	int num_customer=0,item;
	/* Go through each customer one by one */
	while (scanf("%d %d",&(all_orders[num_customer].customer_id),
		&(all_orders[num_customer].item_num))==2){
		*total_items+=all_orders[num_customer].item_num;
		/* Items ordered by this customer */
		for (item=0;item<all_orders[num_customer].item_num;item++){
			scanf("%d %d%c",&(all_orders[num_customer].item_id),
    			&(all_orders[num_customer].items[item].bin_num),
    			&(all_orders[num_customer].items[item].corridor_num));
		}
	num_customer++;
	}
   	return num_customer;
}

/* Function items_in_order sorts the array all_orders which stores the orders
by all customers into a particular order specified in assignment sheet */
void items_in_order(order all_orders[],int num_customer){
	int each_customer,item,i,previous_bin,bin;
	char previous_corridor,corridor;
	for (each_customer=0;each_customer<num_customer;each_customer++){
		for (item=1;item<all_orders[each_customer].item_num;item++){
			/* Arrange by the order of the corridors */
			for (i=item-1;i>=0&&
				all_orders[each_customer].items[i+1].corridor_num<
				all_orders[each_customer].items[i].corridor_num;i--){
				assign_values(all_orders,each_customer,i,
					&previous_corridor,&corridor,&previous_bin,&bin);
				item_swap(all_orders,each_customer,i,previous_corridor,
					corridor,previous_bin,bin);
			}
			assign_values(all_orders,each_customer,i,&previous_corridor,
				&corridor,&previous_bin,&bin);
			/* Arrange by the order of the bins */
			while (corridor==previous_corridor&&i>=0){
				/* In the cases the bins are numbered in decreasing order
				the bins are numbered in increasing order */
				if ((corridor%2==0&&bin>previous_bin)||
					(corridor%2!=0&&bin<previous_bin)){
					item_swap(all_orders,each_customer,i,previous_corridor,
						corridor,previous_bin,bin);
					i--;
					assign_values(all_orders,each_customer,i,
						&previous_corridor,&corridor,&previous_bin,&bin);
				}else{
					break;
				}
			}
		}
	}
}

/* The function item_swap simply swapS the order of two adjecent items */
void item_swap(order all_orders[],int each_customer,int i,
	char previous_corridor,char corridor,int previous_bin,int bin){
	all_orders[each_customer].items[i].corridor_num = corridor;
	all_orders[each_customer].items[i+1].corridor_num = previous_corridor;
	all_orders[each_customer].items[i].bin_num = bin;
	all_orders[each_customer].items[i+1].bin_num = previous_bin;
}

/* The function assign implements the new order of items after swapping
it passes information about two adjecent items as pointers */
void assign_values(order all_orders[],int each_customer,int i,
	char *previous_corridor,char *corridor,int *previous_bin,int *bin){
	*previous_corridor = all_orders[each_customer].items[i].corridor_num;
	*corridor = all_orders[each_customer].items[i+1].corridor_num;
	*previous_bin = all_orders[each_customer].items[i].bin_num;
	*bin = all_orders[each_customer].items[i+1].bin_num;
}

/* Function count_distance calculates distances for items to be taken for each
order and store them customer by customer into an array distance which is
passesed as a pointer, it returns the total distance for picking all orders */
double count_distance(order all_orders[],int num_customer,int row,int column,
		double distance[]){
	/* total_corridor records number of corridor pickers travel through */
	int total_corridor,each_customer,each_item;
	double total_distance=0.0;
	char previous_corridor,corridor;
	/* Iterate through every customer and then every item */
	for (each_customer=0;each_customer<num_customer;each_customer++){
		total_corridor = 1;
		/* In the case that there is only one item purchased */
		if (all_orders[each_customer].item_num==1){
			total_corridor++;
		}
		for (each_item=1;each_item<all_orders[each_customer].item_num;
				each_item++){
			previous_corridor=all_orders[each_customer]
			.items[each_item-1].corridor_num;
			corridor = all_orders[each_customer].items[each_item].corridor_num;
			/* First item is in the corridor that is directed upwards */
			if (each_item==1&&previous_corridor%2==0){
				total_corridor++;
			}
			/* When only last item left and the corridor directs downwards */
			if (each_item==all_orders[each_customer].item_num-1
				&&corridor%2!=0){
				total_corridor++;
			}
			total_corridor += calcualte_corridor(corridor,previous_corridor);
		}
		/* Calculate the distance travelled using total_corridor which is the
		number of corridors travelled trough */
		distance[each_customer] = (1+total_corridor*(row+1))*BIN_DISTANCE
        	+(column-1)*CORRIDOR_DISTANCE;
		total_distance += distance[each_customer];
    }
	return total_distance;
}

/* the function calculates number of corridor if next item is in a different
corridor from the last item, the number of corridor picker has to travel
through depends on the required travelling direction of the corridor */
int calcualte_corridor(char corridor,char previous_corridor){
	int total_corridor=0;
	if (corridor!=previous_corridor){
		/* The corridor is directed in opposite direction */
		if ((corridor-previous_corridor)%2!=0){
			total_corridor++;
		/* The corridor is in the same direction */
		}else{
			total_corridor += 2;
		}
	}
	return total_corridor;
}

/* The function counts and returns the number of picker required for all
orders and pass an array of type of order, new_orders */
int count_picker(order all_orders[],int num_customer,order new_orders[]){
	int each_customer,num_picker=0;
	for (each_customer=0;each_customer<num_customer;each_customer++){
		/* Only items ordered by last customer left */
		if (each_customer==num_customer-1){
			combine_orders(all_orders,each_customer,new_orders,num_picker);
			num_picker++;
		}else{
			each_customer=combine_orders(all_orders,each_customer,new_orders,
					num_picker);
			num_picker++;
		}
	}
	return num_picker;
}

/* In the case where a picker can take two orders at the same time, the
function combines both orders and store them into an array of type of order
the fucntion then returns incrementing index of customers */
int combine_orders(order all_orders[],int each_customer,order new_orders[],
		int num_picker){
	int new_item_num=0,each_item,current_item_num,next_item_num;
	/* Iterate through every item, given a particular customer, it puts items
	into a new_order picking list, indexed using new_item_num */
	for (each_item=0;each_item<all_orders[each_customer].item_num;
		each_item++){
		new_orders[num_picker].items[new_item_num]
		= all_orders[each_customer].items[each_item];
		new_item_num++;
	}
	/* If the number of items from next order added to that from current
	order is smaller than 10, put those items in current picking list */
	current_item_num = all_orders[each_customer].item_num;
	next_item_num = all_orders[each_customer+1].item_num;
	if (current_item_num+next_item_num<=10){
		for (each_item=0;each_item<next_item_num;each_item++){
			new_orders[num_picker].items[new_item_num]
			= all_orders[each_customer+1].items[each_item];
			new_item_num++;
		}
		each_customer++;
	}
	new_orders[num_picker].item_num = new_item_num;
	return each_customer;
}
