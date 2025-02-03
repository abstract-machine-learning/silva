/**
 * Dataset.
 * 
 * Defines some utility function to manipulate a dataset.
 * 
 * @file dataset.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "dataset.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/** Size of buffer. */
#define LABEL_BUFFER_SIZE 32


/** Structure of a dataset. */
struct dataset {
    unsigned int size;        /**< Number of samples. */
    unsigned int space_size;  /**< Size of the feature space. */
    double *data;               /**< Features (row major matrix). */
    char *labels;             /**< Labels. */
};



/***********************************************************************
 * Internal functions.
 **********************************************************************/

/**
 * Consumed whitspaces from beginning of stream.
 *
 * @param[in,out] stream Stream
 */
static void clear_stream(FILE *stream) {
    int c;

    do {
        c = fgetc(stream);
    }
    while (isspace(c));
    ungetc(c, stream);
}



/**
 * Parses a dataset header.
 *
 * @param[out] format Dataset format
 * @param[out] n_rows Number of rows
 * @param[out] n_cols Number of columns (excluding label)
 * @param[in,out] stream Stream
 */
static void parse_header(
    DatasetFormat *format,
    unsigned int *n_rows,
    unsigned int *n_cols,
    FILE *stream
) {
    const long initial_position = ftell(stream);
    unsigned int input_1, input_2;
    char next;
    int result;
    
    result = fscanf(stream, "# %u %u", &input_1, &input_2);
    if (result != 2) {
        fprintf(stderr, "[%s: %d] Cannot parse header.\n", __FILE__, __LINE__);
        abort();
    }

    next = fgetc(stream);
    fseek(stream, initial_position, SEEK_SET);

    switch (next) {
        case ' ':
            result = fscanf(stream, "# %u %u %u", format, n_rows, n_cols);
            if (result != 3) {
                fprintf(stderr, "[%s: %d] Cannot parse header.\n", __FILE__, __LINE__);
                abort();
            }
            break;

        case '\n':
        case '\r':
            *format = DATASET_CSV;
            result = fscanf(stream, "# %u %u", n_rows, n_cols);
            if (result != 2) {
                fprintf(stderr, "[%s: %d] Cannot parse header.\n", __FILE__, __LINE__);
                abort();
            }
            break;

        default:
            fprintf(stderr, "[%s: %d] Cannot parse header.\n", __FILE__, __LINE__);
            abort();
    }

    clear_stream(stream);
}



/**
 * Reads a dataset in CSV format.
 *
 * @param[in,out] stream Stream
 * @return Dataset
 */
static Dataset dataset_read_csv(FILE *stream) {
    char *labels;
    double *data;
    Dataset dataset;
    unsigned int n_cols, n_rows, i, j, result;
    DatasetFormat format;


    parse_header(&format, &n_rows, &n_cols, stream);

    labels = (char *) malloc(LABEL_BUFFER_SIZE * n_rows * sizeof(char));
    data = (double *) malloc(n_rows * n_cols * sizeof(double));

    for (i = 0; i < n_rows; ++i) {
        double buffer;
        memset(labels + i * LABEL_BUFFER_SIZE, 0, LABEL_BUFFER_SIZE * sizeof(char));
        result = fscanf(stream, "\n%[^,],", labels + (i * LABEL_BUFFER_SIZE));
        for (j = 0; j < n_cols - 1; ++j) {
            result = fscanf(stream, "%lf,", &buffer);
            data[i * n_cols + j] = buffer;
        }

        result = fscanf(stream, "%lf", &buffer);
        data[i * n_cols + j] = buffer;
    }

    dataset = (Dataset) malloc(sizeof(struct dataset));
    if (!dataset) {
        fprintf(stderr, "[%s: %d] Cannot parse header.\n", __FILE__, __LINE__);
        abort();
    }

    dataset->size = n_rows;
    dataset->space_size = n_cols;
    dataset->data = data;
    dataset->labels = labels;

    (void) result;
    return dataset;
}



/**
 * Reads a dataset in binary format.
 *
 * @param[in,out] stream Stream
 * @return Dataset
 */
static Dataset dataset_read_binary(FILE *stream) {
    Dataset dataset;
    DatasetFormat format;
    unsigned int i, n_rows, n_cols;
    char *labels;
    double *data;
    size_t n_read;

    parse_header(&format, &n_rows, &n_cols, stream);


    dataset = (Dataset) malloc(sizeof(struct dataset));
    labels = (char *) malloc(LABEL_BUFFER_SIZE * n_rows * sizeof(char));
    data = (double *) malloc(n_rows * n_cols * sizeof(double));

    for (i = 0; i < n_rows; ++i) {
        n_read = fread(labels + i * LABEL_BUFFER_SIZE, sizeof(char), LABEL_BUFFER_SIZE, stream);
        if (n_read != LABEL_BUFFER_SIZE) {
            fprintf(stderr, "[%s: %d] Cannot read dataset.\n", __FILE__, __LINE__);
            abort();
        }
        n_read = fread(data + i * n_cols, sizeof(double), n_cols, stream);
        if (n_read != n_cols) {
            fprintf(stderr, "[%s: %d] Cannot read dataset.\n", __FILE__, __LINE__);
            abort();
        }
    }

    dataset->size = n_rows;
    dataset->space_size = n_cols;
    dataset->data = data;
    dataset->labels = labels;

    return dataset;
}



/**
 * Writes a dataset in CSV format.
 *
 * @param[in] dataset Dataset
 * @param[in,out] stream Stream
 * @return Dataset
 */
static void dataset_write_csv(const Dataset dataset, FILE *stream) {
    const unsigned int space_size = dataset->space_size;
    unsigned int i, j;

    fprintf(stream, "# %u %u %u\n", DATASET_CSV, dataset->size, dataset->space_size);

    for (i = 0; i < dataset->size; ++i) {
        fprintf(stream, "%s", dataset->labels + i * LABEL_BUFFER_SIZE);
        for (j = 0; j < space_size; ++j) {
            fprintf(stream, ",%g", dataset->data[i * space_size + j]);
        }
        fprintf(stream, "\n");
    }
}



/**
 * Writes a dataset in binary format.
 *
 * @param[in] dataset Dataset
 * @param[in,out] stream Stream
 * @return Dataset
 */
static void dataset_write_binary(const Dataset dataset, FILE *stream) {
    const unsigned int size = dataset->size,
                       space_size = dataset->space_size;
    unsigned int i;

    fprintf(stream, "# %u %u %u\n", DATASET_BINARY, size, space_size);

    for (i = 0; i < size; ++i) {
        fwrite(dataset->labels + i * LABEL_BUFFER_SIZE, sizeof(char), LABEL_BUFFER_SIZE, stream);
        fwrite(dataset->data + i * space_size, sizeof(double), space_size, stream);
    }
}


/***********************************************************************
 * Public functions.
 **********************************************************************/

Dataset dataset_read(FILE *stream) {
    unsigned int n_rows, n_cols;
    DatasetFormat format;
    long initial_position;

    if (!stream) {
        fprintf(stderr, "[%s: %d] Cannot read dataset file.\n", __FILE__, __LINE__);
        abort();
    }

    initial_position = ftell(stream);
    parse_header(&format, &n_rows, &n_cols, stream);
    fseek(stream, initial_position, SEEK_SET);

    switch (format) {
        case DATASET_CSV:
            return dataset_read_csv(stream);

        case DATASET_BINARY:
            return dataset_read_binary(stream);
    }

    fprintf(stderr, "[%s: %d] Cannot read dataset file.\n", __FILE__, __LINE__);
    abort();
}



void dataset_write(
    const Dataset dataset,
    const DatasetFormat format,
    FILE *stream
) {
    switch (format) {
        case DATASET_CSV:
            dataset_write_csv(dataset, stream);
            break;

        case DATASET_BINARY:
            dataset_write_binary(dataset, stream);
            break;
    }

    fprintf(stderr, "[%s: %d] Unsupporteddataset format.\n", __FILE__, __LINE__);
    abort();
}



void dataset_delete(Dataset *dataset) {
    if (dataset == NULL || *dataset == NULL) {
        return;
    }

    free((*dataset)->data);
    free((*dataset)->labels);
    free(*dataset);
    *dataset = NULL;
}


unsigned int dataset_get_size(const Dataset dataset) {
    return dataset->size;
}


unsigned int dataset_get_space_size(const Dataset dataset) {
    return dataset->space_size;
}


double *dataset_get_row(const Dataset dataset, const unsigned int i) {
    return dataset->data + i * dataset->space_size;
}


char *dataset_get_label(const Dataset dataset, const unsigned int i) {
    return dataset->labels + i * LABEL_BUFFER_SIZE;
}
