#ifndef PROJECT2_ANALYZE_H
#define PROJECT2_ANALYZE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "graph_generator.h"
#include "dijkstra.h"

typedef struct AnalysisResult {
    int num_vertices;
    int num_edges;
    int compare_count;
} AnalysisResult;

typedef struct AnalysisResultList {
    AnalysisResult *results;
    int size;
    int capacity;
} AnalysisResultList;

AnalysisResultList *init_result_list() {
    AnalysisResultList *result_list = (AnalysisResultList *) malloc(sizeof(AnalysisResultList));
    result_list->size = 0;
    result_list->capacity = 10;
    result_list->results = (AnalysisResult *) malloc(result_list->capacity * sizeof(AnalysisResult));
    return result_list;
}

void add_result(AnalysisResultList *result_list, AnalysisResult result) {
    if (result_list->size == result_list->capacity) {
        result_list->capacity *= 2;
        result_list->results = (AnalysisResult *) realloc(result_list->results,
                                                          result_list->capacity * sizeof(AnalysisResult));
        if (result_list->results == NULL) {
            fprintf(stderr, "Error: realloc failed\n");
            exit(EXIT_FAILURE);
        }
    }
    result_list->results[result_list->size++] = result;
}

void export_result(AnalysisResultList *result_list, const char *file_name) {
    FILE *file = fopen(file_name, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: cannot open file %s\n", file_name);
        exit(EXIT_FAILURE);
    }
    fprintf(file, "num_vertices,num_edges,compare_count\n");
    for (int i = 0; i < result_list->size; ++i) {
        fprintf(file, "%d,%d,%d\n", result_list->results[i].num_vertices, result_list->results[i].num_edges,
                result_list->results[i].compare_count);
    }
    fclose(file);
}

int analyze_adj_matrix(int num_vertices, int num_edges) {
    Graph *graph = random_graph_adj_matrix(num_vertices, num_edges);
    int compare_count = 0;
    dijkstra_adj_matrix(graph, &compare_count);
    return compare_count;
}

int analyze_adj_list(int num_vertices, int num_edges) {
    Graph *graph = random_graph_adj_list(num_vertices, num_edges);
    int compare_count = 0;
    dijkstra_adj_list(graph, &compare_count);
    return compare_count;
}

typedef int (*AnalyzeFunction)(int, int);

void analyze_with_different_edges(int num_vertices, int num_edges_start, int num_edges_end,
                                  int num_edges_step, int num_trials,
                                  AnalysisResultList *result_list, AnalyzeFunction analyze) {
    for (int i = num_edges_start; i <= num_edges_end; i += num_edges_step) {
        printf("num_vertices: %d, num_edges: %d\n", num_vertices, i);
        fflush(stdout);
        for (int j = 0; j < num_trials; ++j) {
            AnalysisResult result;
            result.num_vertices = num_vertices;
            result.num_edges = i;
            result.compare_count = analyze(num_vertices, i);
            add_result(result_list, result);
        }
    }
}

void analyze_with_different_method(int num_vertices_start,
                                   int num_vertices_end,
                                   int num_vertices_step,
                                   int num_edges_start,
                                   int num_edges_end,
                                   int num_edges_step,
                                   int num_trials,
                                   AnalysisResultList *result_list,
                                   AnalyzeFunction analyze) {
    for (int i = num_vertices_start; i <= num_vertices_end; i += num_vertices_step) {
        printf("num_vertices: %d\n", i);
        fflush(stdout);
        analyze_with_different_edges(i, num_edges_start, num_edges_end, num_edges_step, num_trials, result_list,
                                     analyze);
    }
}

void analyze_diff_edge_matrix() {
    const char *file_name = "diff_edge_matrix.csv";
    const int NUM_VERTICES = 5000;
    const int NUM_EDGES_START = 10000;
    const int NUM_EDGES_END = 1000000;
    const int NUM_EDGES_STEP = 10000;
    const int NUM_TRIALS = 10;
    AnalysisResultList *result_list = init_result_list();
    analyze_with_different_edges(NUM_VERTICES, NUM_EDGES_START, NUM_EDGES_END, NUM_EDGES_STEP, NUM_TRIALS,
                                 result_list, analyze_adj_matrix);
    export_result(result_list, file_name);
}

void analyze_diff_vertex_matrix() {
    const char *file_name = "diff_vertex_matrix.csv";
    const int NUM_VERTICES_START = 100;
    const int NUM_VERTICES_END = 5000;
    const int NUM_VERTICES_STEP = 100;
    const int NUM_EDGES = 10000;
    const int NUM_TRIALS = 10;
    AnalysisResultList *result_list = init_result_list();
    analyze_with_different_method(NUM_VERTICES_START, NUM_VERTICES_END, NUM_VERTICES_STEP, NUM_EDGES, NUM_EDGES,
                                  NUM_EDGES, NUM_TRIALS, result_list, analyze_adj_matrix);
    export_result(result_list, file_name);
}

void analyze_diff_edge_list() {
    const char *file_name = "diff_edge_list.csv";
    const int NUM_VERTICES = 100000;
    const int NUM_EDGES_START = 10000;
    const int NUM_EDGES_END = 1000000;
    const int NUM_EDGES_STEP = 10000;
    const int NUM_TRIALS = 10;
    AnalysisResultList *result_list = init_result_list();
    analyze_with_different_edges(NUM_VERTICES, NUM_EDGES_START, NUM_EDGES_END, NUM_EDGES_STEP, NUM_TRIALS,
                                 result_list, analyze_adj_list);
    export_result(result_list, file_name);
}

void analyze_diff_vertex_list() {
    const char *file_name = "diff_vertex_list.csv";
    const int NUM_VERTICES_START = 100000;
    const int NUM_VERTICES_END = 1000000;
    const int NUM_VERTICES_STEP = 10000;
    const int NUM_EDGES = 2000000;
    const int NUM_TRIALS = 10;
    AnalysisResultList *result_list = init_result_list();
    analyze_with_different_method(NUM_VERTICES_START, NUM_VERTICES_END, NUM_VERTICES_STEP, NUM_EDGES, NUM_EDGES,
                                  NUM_EDGES, NUM_TRIALS, result_list, analyze_adj_list);
    export_result(result_list, file_name);
}

void analyze() {
    analyze_diff_edge_matrix();
    analyze_diff_vertex_matrix();
    analyze_diff_edge_list();
    analyze_diff_vertex_list();
}

#endif //PROJECT2_ANALYZE_H
