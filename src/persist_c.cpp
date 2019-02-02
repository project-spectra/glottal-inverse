#include "persist_c.h"
#include "amgif.h"


void findComputeStatus(ComputeStatus& toLoad, ComputeStatus& toStore) {
    char path[PERSIST_PATHLEN];

    bool dirExists = fs::is_directory(PERSIST_DIR);
   
    constexpr size_t length = 2 << J;
    size_t mu;

    for (mu = 0; mu < length; ++mu) {
        snprintf(path, PERSIST_PATHLEN, "%s/%zu", PERSIST_DIR, mu);

        if (dirExists && fs::exists(path) && fs::is_regular_file(path)) {
            toLoad.push_back(std::make_pair(mu, path));
        } else {
            toStore.push_back(std::make_pair(mu, path));
        }
    }
}


gsl_spmatrix *smartGetC(size_t mu, const std::string& path, bool load) {
    constexpr size_t length = 2 << J;
    FILE *f;
    gsl_spmatrix *C;

    if (load) {
        // if the directory already exists, simply restore.
        f = fopen(path.c_str(), "rb");
        C = bz2readC(f);
        fclose(f);
    } else {
        // else, compute and then persist.
        fs::create_directories(PERSIST_DIR);

        C = gsl_spmatrix_alloc(length, length);

        computeSingleC(C, mu);

        f = fopen(path.c_str(), "wb");
        bz2writeC(f, C);
        fclose(f);
    }
    
    return C;
}

struct buf_triplet {
    size_t i;
    size_t j;
    double val;
};

struct buf_header {
    size_t n_rows;
    size_t n_cols;
    size_t nzmax;
};

gsl_spmatrix *bz2readC(FILE *f) {
    BZFILE *b;
    int bzerr;

    b = BZ2_bzReadOpen(&bzerr, f, 0, 1, NULL, 0);
    if (bzerr != BZ_OK) {
        BZ2_bzReadClose(&bzerr, b);
        throw std::ios_base::failure("Can't decompress file");
    }

    struct buf_header header;
    struct buf_triplet triplet;
    gsl_spmatrix *C;

    // read the header
    bzerr = BZ_OK;
    BZ2_bzRead(&bzerr, b, &header, sizeof header);
    if (bzerr != BZ_OK) {
        BZ2_bzReadClose(&bzerr, b);
        throw std::ios_base::failure("Can't decompress file header");
    }

    C = gsl_spmatrix_alloc_nzmax(
            header.n_rows,
            header.n_cols,
            header.nzmax,
            GSL_SPMATRIX_TRIPLET
    );

    bzerr = BZ_OK;
    while (bzerr == BZ_OK) {
        BZ2_bzRead(&bzerr, b, &triplet, sizeof triplet);
        if (bzerr == BZ_OK) {
            gsl_spmatrix_set(C, triplet.i, triplet.j, triplet.val);
        }
    }
    
    if (bzerr != BZ_STREAM_END) {
        BZ2_bzReadClose(&bzerr, b);
        throw std::ios_base::failure("Couldn't decompress file triplet");
    }

    BZ2_bzReadClose(&bzerr, b);

    return C;
}

void bz2writeC(FILE *f, gsl_spmatrix *C) {
    BZFILE *b;
    int bzerr;

    b = BZ2_bzWriteOpen(&bzerr, f, 5, 1, 20);
    if (bzerr != BZ_OK) {
        BZ2_bzWriteClose(&bzerr, b, 0, nullptr, nullptr);
        throw std::ios_base::failure("Can't compress file");
    }

    struct buf_header header;
    struct buf_triplet triplet;

    header.n_rows = C->size1;
    header.n_cols = C->size2;
    header.nzmax = gsl_spmatrix_nnz(C);

    // write the header
    bzerr = BZ_OK;
    BZ2_bzWrite(&bzerr, b, &header, sizeof header);
    if (bzerr != BZ_OK) {
        BZ2_bzWriteClose(&bzerr, b, 0, nullptr, nullptr);
        throw std::ios_base::failure("Can't compress file header");
    }

    for (triplet.i = 0; triplet.i < C->size1; ++triplet.i) {
        for (triplet.j = 0; triplet.j < C->size2; ++triplet.j) {
            // if triplet is nonzero

            BZ2_bzWrite(&bzerr, b, &triplet, sizeof triplet);
            if (bzerr != BZ_OK) { 
                BZ2_bzWriteClose(&bzerr, b, 0, nullptr, nullptr);
                throw std::ios_base::failure("Can't compress file triplet");
            }
        }
    }
    
    BZ2_bzWriteClose(&bzerr, b, 0, nullptr, nullptr);
}
