/*
 * Copyright (c) 2012 Stephan Hagios <stephan.hagios@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "TaskLists.h"

void addItemToTaskLists_Lists(TaskLists_Lists *taskLists_Lists, TaskListItem *item)
{
    void *tmp;
    if (taskLists_Lists->numberItems != 0)
    {
        tmp = realloc(taskLists_Lists->items, ((taskLists_Lists->numberItems + 1) * sizeof (TaskListItem)));
        if (!tmp)
        {
            printf("ERROR, reallocating");
            return;
        }
    } else
    {
        tmp = malloc(sizeof (TaskListItem));
        if (!tmp)
        {
            printf("ERROR, allocating");
            return;
        }
    }
    taskLists_Lists->items = (TaskListItem*) tmp;
    taskLists_Lists->items[taskLists_Lists->numberItems++] = *item;
}

void deleteItemFromTaskLists_list(TaskLists_Lists *taskLists_Lists, char *id)
{
    int i;
    int indexId = -1;
    TaskLists_Lists *newList = malloc(sizeof (TaskLists_Lists));
    newList->numberItems = 0;

    for (i = 0; i < taskLists_Lists->numberItems; i++)
    {
        if (taskLists_Lists->items[i].id != NULL)
        {
            if (strcmp(taskLists_Lists->items[i].id, id) == 0)
            {
                indexId = i;
                break;
            } else
                addItemToTaskLists_Lists(newList, &taskLists_Lists->items[i]);
        }
    }

    if (indexId == -1)
        return;

    for (i = (indexId + 1); i < taskLists_Lists->numberItems; i++)
    {
        addItemToTaskLists_Lists(newList, &taskLists_Lists->items[i]);
    }
    taskLists_Lists->items = newList->items;
    taskLists_Lists->numberItems = newList->numberItems;
}

TaskLists_Lists* createNewTaskLists_ListsFromJson(json_value *value)
{

    if (value != NULL)
    {
        TaskLists_Lists *newList = malloc(sizeof (TaskLists_Lists));
        int i, j;
        for (i = 0; i < value->u.object.length; i++)
        {
            if (strcmp(value->u.object.values[i].name, KIND_STRING) == 0)
            {
                newList->kind = malloc(sizeof (TASKLISTS_STRING));
                strcpy(newList->kind, TASKLISTS_STRING);
            } else if (strcmp(value->u.object.values[i].name, ETAG_STRING) == 0)
            {
                newList->etag = malloc(value->u.object.values[i].value->u.string.length + 1);
                strcpy(newList->etag, value->u.object.values[i].value->u.string.ptr);
            } else if (strcmp(value->u.object.values[i].name, NEXTPAGETOKEN_STRING) == 0)
            {
                newList->nextPageToken = malloc(value->u.object.values[i].value->u.string.length + 1);
                strcpy(newList->nextPageToken, value->u.object.values[i].value->u.string.ptr);
            } else if (value->u.object.values[i].value->type == json_array)
                for (j = 0; j < value->u.object.values[i].value->u.array.length; j++)
                    addItemToTaskLists_Lists(newList, createNewTaskListItem(value->u.object.values[i].value->u.array.values[j]));
        }
        return newList;
    }
    return NULL;
}

TaskListItem* createNewTaskListItem(json_value * value)
{
    if (value != NULL)
    {
        int i;
        TaskListItem *item = malloc(sizeof (TaskListItem));
        for (i = 0; i < value->u.object.length; i++)
        {
            if (strcmp(value->u.object.values[i].name, KIND_STRING) == 0)
            {
                item->kind = malloc(value->u.object.values[i].value->u.string.length + 1);
                strcpy(item->kind, value->u.object.values[i].value->u.string.ptr);
            } else if (strcmp(value->u.object.values[i].name, ID_STRING) == 0)
            {
                item->id = malloc(value->u.object.values[i].value->u.string.length + 1);
                strcpy(item->id, value->u.object.values[i].value->u.string.ptr);
            } else if (strcmp(value->u.object.values[i].name, ETAG_STRING) == 0)
            {
                item->etag = malloc(value->u.object.values[i].value->u.string.length + 1);
                strcpy(item->etag, value->u.object.values[i].value->u.string.ptr);
            } else if (strcmp(value->u.object.values[i].name, TITLE_STRING) == 0)
            {
                item->title = malloc(value->u.object.values[i].value->u.string.length + 1);
                strcpy(item->title, value->u.object.values[i].value->u.string.ptr);
            } else if (strcmp(value->u.object.values[i].name, SELFLINK_STRING) == 0)
            {
                item->selfLink = malloc(value->u.object.values[i].value->u.string.length + 1);
                strcpy(item->selfLink, value->u.object.values[i].value->u.string.ptr);
            } else if (strcmp(value->u.object.values[i].name, UPDATED_STRING) == 0)
            {
                item->updated = malloc(value->u.object.values[i].value->u.string.length + 1);
                strcpy(item->updated, value->u.object.values[i].value->u.string.ptr);
            }
        }
        return item;
    }
    return NULL;
}

char *taskLists_List(char *access_token, int maxResults /*default = -1*/, char *pageToken)
{
    if (access_token != NULL)
    {
        struct MemoryStruct chunk;

        chunk.memory = malloc(1); /* will be grown as needed by the realloc above */
        chunk.size = 0; /* no data at this point */

        CURL *curl;
        struct curl_slist *headers = NULL;

        int str_lenght = strlen(HEADER_AUTHORIZATION) + 1;
        char *header = malloc(str_lenght * sizeof (char));
        strcpy(header, HEADER_AUTHORIZATION);


        str_lenght += strlen(access_token);
        header = realloc(header, str_lenght);
        strcat(header, access_token);


        headers = curl_slist_append(headers, header);
        if (headers != NULL) printf("\n%s\n", headers->data);

        curl = curl_easy_init();

        if (!curl)
        {
            printf("ERROR");
            return NULL;
        }

        char *response = NULL;

        curl_easy_setopt(curl, CURLOPT_URL, LISTS_HTTP_REQUEST);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, httpsCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);



        curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        return chunk.memory;
    }
    return NULL;
}

/**
 * If server sends a response.
 * @param ptr
 * @param size
 * @param nmemb
 * @param data
 * @return 
 */
size_t static httpsCallback(void *ptr, size_t size, size_t nmemb, void *data)
{

    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *) data;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL)
    {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        exit(EXIT_FAILURE);
    }

    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}