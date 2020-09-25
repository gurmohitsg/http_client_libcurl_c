/*
 *
 * @author Gurmohit Singh
 *
 * The HTTP Client capable of sending GET request
 * Uses libcurl: A free and easy-to-use client-side URL transfer library for c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// buffer structure which can grow
typedef struct {
	char *buffer;
	size_t size;
} Buffer;


void init_buffer(Buffer *);
void clean_buffer(Buffer *mem);

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

int main()
{
	CURL *curl_handle = NULL;
	Buffer buffer;
	CURLcode res;
	long status;

	// initialize buffer
	init_buffer(&buffer);

	// init curl 
	curl_global_init(CURL_GLOBAL_ALL);

	// get curl handle
	curl_handle = curl_easy_init();

	// set URL
	curl_easy_setopt(curl_handle, CURLOPT_URL, "localhost");

	// set write_back function
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);

	// pass buffer to write back call
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&buffer);

	// send the request
	res = curl_easy_perform(curl_handle);
	if(res!= 0)
	{
		printf("Eror: unable to send request\n");
		exit(1);
	}

	// get status if the request
	curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &status);
    if(status != 200)
    {
        printf("error: server responded with status %ld\n", status);
        exit(1);
    }

    // print buffer
    puts(buffer.buffer);

    // clean buffer
    clean_buffer(&buffer);

    // clean up curl
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();


	return 0;
}

void init_buffer(Buffer * mem){

	mem->buffer = (char *) malloc(1);
	mem->size = 0;
}
void clean_buffer(Buffer *mem)
{
	free(mem->buffer);
	mem->buffer = NULL;
	mem->size=0;
}

size_t write_callback(char *response_data, size_t size, size_t nmemb, void *userdata)
{
	size_t response_data_size = size * nmemb;;
	Buffer *mem = (Buffer *)userdata;

	// allocate new buffer 
	mem->buffer = (char *)realloc (mem->buffer, mem->size + response_data_size + 1);
	if(mem->buffer == NULL)
	{
		printf("Out of memory\n");
		return 0;
	}

	// copy the response_data to buffer
	memcpy(&(mem->buffer[mem->size]),response_data, response_data_size);
	mem->size += response_data_size;
	mem->buffer[mem->size] = 0; // terminate the buffer with '\0'

	return response_data_size;
}