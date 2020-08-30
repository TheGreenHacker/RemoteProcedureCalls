#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>

#include "rpc.h"
#include "person_t/person_t.h"
#include "../sentinel.h"
#include "../serialize.h"
#include "ll_t/ll_t.h"
#include "ll_pair_t/ll_pair_t.h"
#include "serialize_int/serialize_int.h"

char *server_ip;

/* Function responsible for communicating with server. */
void rpc_send_recv (rpc_hdr_t *rpc_hdr, ser_buff_t *send_b, ser_buff_t **recv_b){
    struct sockaddr_in dest;
    int sockfd;
    int addr_len;

    dest.sin_family = AF_INET;
    dest.sin_port = htons(PORT);
    dest.sin_addr.s_addr = inet_addr(server_ip);
    
    addr_len = sizeof(struct sockaddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockfd == -1){
        printf("socket creation failed\n");
        return;
    }
    
    /* Send RPC header to server */
    if (sendto(sockfd, rpc_hdr, sizeof(rpc_hdr_t), 0, (struct sockaddr *)&dest, sizeof(struct sockaddr)) == -1) {
        printf("send failed\n");
        return;
    }
    
    /* Send the serialized arguments */
    if (sendto(sockfd, send_b->b, rpc_hdr->payload_size, 0, (struct sockaddr *)&dest, sizeof(struct sockaddr)) == -1) {
        printf("send failed\n");
        return;
    }

    /* Get RPC header from server */
    if (recvfrom(sockfd, rpc_hdr, sizeof(rpc_hdr_t), 0, (struct sockaddr *)&dest, (socklen_t *)&addr_len) == -1) {
        printf("recv failed\n");
        return;
    }
    
    /* Initialize big enough buffer to store results */
    init_serialized_buffer_of_defined_size(recv_b, rpc_hdr->payload_size);
  
    /* Get results from server */
    if (recvfrom(sockfd, (*recv_b)->b, rpc_hdr->payload_size, 0, (struct sockaddr *)&dest, (socklen_t *)&addr_len) == -1) {
        printf("recv failed\n");
        return;
    }
}

ll_t *get_senior_citizens_client_stub_unmarshal(ser_buff_t *recv_b) {
    return deserialize_ll_t(recv_b, deserialize_person_t_wrapper);
}

ser_buff_t *get_senior_citizens_client_stub_marshal(ll_t *l) {
    ser_buff_t *send_b;
    init_serialized_buffer(&send_b);
    
    /* Serialize the list */
    serialize_ll_t(l, send_b, serialize_person_t_wrapper);
    
    return send_b;
}

ll_t *rpc_get_senior_citizens(ll_t *l, rpc_hdr_t *rpc_hdr) {
    /* Serialize (marshal) arguments */
    ser_buff_t *send_b = get_senior_citizens_client_stub_marshal(l);
    
    /* Truncate sent buffer to send as many bytes as serialized arguments are */
    truncate_serialize_buffer(&send_b);
    
    /* Indicate size of payload (RPC arguments) in rpc header */
    rpc_hdr->payload_size = get_serialize_buffer_length(send_b);
    
    ser_buff_t *recv_b;
    
    /* Send serialized arguments to server and wait for reply */
    rpc_send_recv(rpc_hdr, send_b, &recv_b);
    
    /* Free sent buffer containing serialized arguments */
    free_serialize_buffer(send_b);
    
    /* Deserialize (unmarshall) serialized result from server */
    ll_t *senior_citizens = get_senior_citizens_client_stub_unmarshal(recv_b);
    
    /* Free received buffer containing serialized results */
    free_serialize_buffer(recv_b);
    
    return senior_citizens;
}

person_t *get_eldest_citizen_client_stub_unmarshal(ser_buff_t *recv_b) {
    person_t *p = calloc(1, sizeof(person_t));
    
    de_serialize_data((char *)p->name, recv_b, 32 * sizeof(char));
    de_serialize_data((char *)&p->age, recv_b, sizeof(int));
    de_serialize_data((char *)&p->weight, recv_b, sizeof(int));
    
    return p;
}

ser_buff_t *get_eldest_citizen_client_stub_marshal(ll_t *l) {
    ser_buff_t *send_b;
    init_serialized_buffer(&send_b);
    
    /* Serialize the list */
    serialize_ll_t(l, send_b, serialize_person_t_wrapper);
    
    return send_b;
}

person_t *rpc_get_eldest_citizen(ll_t *l, rpc_hdr_t *rpc_hdr) {
    /* Serialize (marshal) arguments */
    ser_buff_t *send_b = get_eldest_citizen_client_stub_marshal(l);
    
    /* Truncate sent buffer to send as many bytes as serialized arguments are */
    truncate_serialize_buffer(&send_b);
    
    /* Indicate size of payload (RPC arguments) in rpc header */
    rpc_hdr->payload_size = get_serialize_buffer_length(send_b);
    
    ser_buff_t *recv_b;
    
    /* Send serialized arguments to server and wait for reply */
    rpc_send_recv(rpc_hdr, send_b, &recv_b);
    
    /* Free sent buffer containing serialized arguments */
    free_serialize_buffer(send_b);
    
    /* Deserialize (unmarshall) serialized result from server */
    person_t *eldest_citizen = get_eldest_citizen_client_stub_unmarshal(recv_b);
    
    /* Free received buffer containing serialized results */
    free_serialize_buffer(recv_b);
    
    return eldest_citizen;
}

int compute_list_sum_client_stub_unmarshal(ser_buff_t *recv_b) {
    int sum;
    de_serialize_data((char *)&sum, recv_b, sizeof(int));
    return sum;
}

ser_buff_t *compute_list_sum_client_stub_marshal(ll_t *l) {
    ser_buff_t *send_b;
    init_serialized_buffer(&send_b);
    
    /* Serialize the list */
    serialize_ll_t(l, send_b, serialize_int_wrapper);
    
    return send_b;
}

int rpc_compute_list_sum(ll_t *l, rpc_hdr_t *rpc_hdr) {
    /* Serialize (marshal) arguments */
    ser_buff_t *send_b = compute_list_sum_client_stub_marshal(l);
    
    /* Truncate sent buffer to send as many bytes as serialized arguments are */
    truncate_serialize_buffer(&send_b);
    
    /* Indicate size of payload (RPC arguments) in rpc header */
    rpc_hdr->payload_size = get_serialize_buffer_length(send_b);
    
    ser_buff_t *recv_b;
    
    /* Send serialized arguments to server and wait for reply */
    rpc_send_recv(rpc_hdr, send_b, &recv_b);
    
    /* Free sent buffer containing serialized arguments */
    free_serialize_buffer(send_b);
    
    /* Deserialize (unmarshall) serialized result from server */
    int sum = compute_list_sum_client_stub_unmarshal(recv_b);
    
    /* Free received buffer containing serialized results */
    free_serialize_buffer(recv_b);
    
    return sum;
}

ll_pair_t *list_splitter_stub_unmarshal(ser_buff_t *recv_b) {
    return deserialize_ll_pair_t(recv_b);
}

ser_buff_t *list_splitter_stub_marshal(ll_t *l) {
    ser_buff_t *send_b;
    init_serialized_buffer(&send_b);
    
    /* Serialize the list */
    serialize_ll_t(l, send_b, serialize_int_wrapper);
    
    return send_b;
}

ll_pair_t *rpc_list_splitter(ll_t *l, rpc_hdr_t *rpc_hdr) {
    /* Serialize (marshal) arguments */
    ser_buff_t *send_b = list_splitter_stub_marshal(l);
    
    /* Truncate sent buffer to send as many bytes as serialized arguments are */
    truncate_serialize_buffer(&send_b);
    
    /* Indicate size of payload (RPC arguments) in rpc header */
    rpc_hdr->payload_size = get_serialize_buffer_length(send_b);
    
    ser_buff_t *recv_b;
    
    /* Send serialized arguments to server and wait for reply */
    rpc_send_recv(rpc_hdr, send_b, &recv_b);
    
    /* Free sent buffer containing serialized arguments */
    free_serialize_buffer(send_b);
    
    /* Deserialize (unmarshall) serialized result from server */
    ll_pair_t *ll_pair = list_splitter_stub_unmarshal(recv_b);
    
    /* Free received buffer containing serialized results */
    free_serialize_buffer(recv_b);
    
    return ll_pair;
}


int main(int argc, char **argv) {
    int rpc_id;
    
    server_ip = (argc > 1) ? argv[1] : "127.0.0.1";
    
    printf("RPC options: \n");
    printf("1.GET_SENIOR_CITIZENS \n");
    printf("2.GET_ELDEST_PERSON \n");
    printf("3.COMPUTE_LIST_SUM \n");
    printf("4.LIST_SPLITTER \n");
    
    printf("Enter option: ");
    scanf("%d", &rpc_id);
    
    ll_t *people = init_list();
    ll_t *nums = init_list();
    
    /* Initialize some sample data of type person_t */
    person_t p1, p2, p3, p4, p5;
    strncpy(p1.name, "Jack", 4);
    p1.age = 22;
    p1.weight = 138;
    
    strncpy(p2.name, "David", 5);
    p2.age = 62;
    p2.weight = 145;
    
    strncpy(p3.name, "Jerry", 5);
    p3.age = 16;
    p3.weight = 100;
    
    strncpy(p4.name, "Joe", 3);
    p4.age = 70;
    p4.weight = 155;
    
    strncpy(p5.name, "Donald", 6);
    p5.age = 72;
    p5.weight = 240;
    
    add_data(people, &p1);
    add_data(people, &p2);
    add_data(people, &p3);
    add_data(people, &p4);
    add_data(people, &p5);
    
    int x1, x2, x3, x4, x5, x6;
    x1 = 3;
    x2 = 4;
    x3 = -1;
    x4 = 10;
    x5 = 7;
    x6 = -8;
    
    add_data(nums, &x1);
    add_data(nums, &x2);
    add_data(nums, &x3);
    add_data(nums, &x4);
    add_data(nums, &x5);
    add_data(nums, &x6);
    
    ll_node_t *curr;
    person_t *p;
    
    ll_t *senior_citizens;
    int sum, n;
    ll_pair_t *ll_pair;
    
    rpc_hdr_t rpc_hdr;
    rpc_hdr.rpc_id = rpc_id;
    
    switch (rpc_hdr.rpc_id) {
        case GET_SENIOR_CITIZENS:
            senior_citizens = rpc_get_senior_citizens(people, &rpc_hdr);
            curr = senior_citizens->head;
            
            while (curr) {
                p = (person_t *)curr->data;
                assert(p->age >= 60);
                curr = curr->next;
            }
            
            deinit_list(senior_citizens);
            printf("RPC GET_SENIOR_CITIZENS works!\n");
            break;
        case GET_ELDEST_PERSON:
            p = rpc_get_eldest_citizen(people, &rpc_hdr);
            
            assert(!strncmp(p->name, "Donald", 32));
            assert(p->age == 72);
            assert(p->weight == 240);
            
            free(p);
            printf("RPC GET_ELDEST_PERSON works!\n");
            break;
        case COMPUTE_LIST_SUM:
            sum = rpc_compute_list_sum(nums, &rpc_hdr);
            assert(sum == 15);
            printf("RPC COMPUTE_LIST_SUM works!\n");
            break;
        case LIST_SPLITTER:
            ll_pair = rpc_list_splitter(nums, &rpc_hdr);
            curr = ll_pair->even_lst->head;
            
            while (curr) {
                n = *((int *)curr->data);
                printf("Even list contains node with data %d\n", n);
                assert(n % 2 == 0);
                curr = curr->next;
            }
            
            curr = ll_pair->odd_lst->head;
            while (curr) {
                n = *((int *)curr->data);
                printf("Odd list contains node with data %d\n", n);
                assert(n % 2 == 1 || n % 2 == -1);
                curr = curr->next;
            }
            
            printf("RPC LIST_SPLITTER works!\n");
            break;
        default:
            printf("Invalid option: exiting\n");
            return 1;
    }
    
    /* Clean up resources */
    deinit_list(people);
    deinit_list(nums);
    
    return 0;
}
