#include "operators.h"
#include "operators_restore.h"


void findValidCs(vector<bool>& toStore) {
  
    bool dirExists = fs::is_directory(PERSIST_DIR);

    std::ostringstream out;
    out << PERSIST_DIR << "/C_";

    const auto sep = out.tellp();

    toStore.resize(basisLength);

    for (size_t mu = 0; mu < basisLength; ++mu) {
        out.seekp(sep) << mu << ".gz" << std::flush;
  
        const auto path = out.str();

        toStore[mu] = !dirExists || !fs::exists(path) || !fs::is_regular_file(path);
    }

}
