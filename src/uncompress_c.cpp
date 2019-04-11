#include "restore_c.h"


gsl_spmatrix *cpReadMat(FILE *file) {

    size_t dataSize, compressedSize;
   
    fread(&dataSize, sizeof(size_t), 1, file);        // data
    fread(&compressedSize, sizeof(size_t), 1, file);  // compressed
    
    auto buffer = make_unique<unsigned char[]>(compressedSize);
    auto data = make_unique<unsigned char[]>(dataSize);

    fread(buffer.get(), sizeof(unsigned char), compressedSize, file);

    // Try to uncompress.

    int ret = uncompress(data.get(), &dataSize, buffer.get(), compressedSize);
    if (ret != Z_OK) {
        fclose(file);
        throw std::ios_base::failure("Couldn't decompress matrix");
    }

    struct {
      size_t size1, size2;
      size_t nzmax, nz;
    } nums = {};

    struct {
      size_t *i;
      double *data;
      size_t *p;
      void *work;
    } arrs = {};

    memcpy(&nums, data.get(), sizeof nums);

    arrs.i = new size_t[nums.nz];
    arrs.data = new double[nums.nz];
    arrs.p = new size_t[nums.size2 + 1];

    size_t workSize = GSL_MAX(nums.size1, nums.size1) *
                      GSL_MAX(sizeof(size_t), sizeof(double));

    arrs.work = malloc(workSize);

    memcpy(arrs.i, data.get(), nums.nz * sizeof(size_t));
    memcpy(arrs.data, data.get(), nums.nz * sizeof(double));
    memcpy(arrs.p, data.get(), (nums.size2 + 1) * sizeof(size_t));
    memcpy(arrs.work, data.get(), workSize);

    auto mat = new gsl_spmatrix;
    mat->size1 = nums.size1;
    mat->size2 = nums.size2;
    mat->i = arrs.i;
    mat->data = arrs.data;
    mat->p = arrs.p;
    mat->nzmax = nums.nzmax;
    mat->nz = nums.nz;
    mat->tree_data = nullptr;
    mat->work = arrs.work;
    mat->sptype = GSL_SPMATRIX_CCS;

    return mat;
}

