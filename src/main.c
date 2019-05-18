#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN3(a, b, c)                                                          \
  ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

typedef struct pkg {
  const char *name;
  const char *url;
} pkg_t;

static const char *usage_ =
    "\n  libelli/lib <command> [options]\n\n  Options:\n"
    "    -h, --help     Output this message\n"
    "    -v, --version  Output version information\n\n"
    "  Commands:\n"
    "    i, install NAME [NAME...]  Install one or more packages\n"
    "    s, search QUERY [N]        Search for packages\n"
    "    help [cmd]                 Output help for cmd\n";

static const char *version_ = "libelli 0.0";

static unsigned num_packages_ = 6;
static pkg_t packages_[] = {
    {"all", "https://github.com/libelli"},
    {"std:varray", "https://github.com/libelli/varray"},
    {"std:argparse", "https://github.com/libelli/argparse"},
    {"std:flag", "https://github.com/libelli/flag"},
    {"std:list", "https://github.com/libelli/list"},
    {"math:vector", "https://github.com/libelli/vector"},
    {"math:matrix", "https://github.com/libelli/matrix"}};

bool is_digit(const char *str) {
  for (unsigned i = 0; i < strlen(str); ++i) {
    if (str[i] < 48 || str[i] > 57)
      return false;
  }
  return true;
}

void split_pkg_name(unsigned pkg, unsigned n, char *namespace_buff,
                    char *name_buff) {
  for (unsigned i = 0; i < n; ++i) {
    namespace_buff[i] = 0;
    name_buff[i] = 0;
  }
  unsigned index = 0;
  for (unsigned i = 0; i < strlen(packages_[pkg].name); ++i) {
    if (packages_[pkg].name[i] == ':')
      index = i;
  }
  for (unsigned i = 0; i < index; ++i) {
    namespace_buff[i] = packages_[pkg].name[i];
  }
  namespace_buff[index] = 0;
  if (index != 0) {
    for (unsigned i = index + 1; i < strlen(packages_[pkg].name); ++i) {
      name_buff[i - index - 1] = packages_[pkg].name[i];
    }
  } else {
    for (unsigned i = 0; i < strlen(packages_[pkg].name); ++i) {
      name_buff[i] = packages_[pkg].name[i];
    }
  }
  name_buff[strlen(packages_[pkg].name)] = 0;
}

void print_package(unsigned pkg, unsigned index) {
  char namespace[128], name[128];
  split_pkg_name(pkg, 128, namespace, name);
  printf("\033[1;30m%2u\033[0m \033[1;34m%s\033[1;30m:\033[1;33m%s\033[0m\n",
         index, namespace, name);
  printf("     \033[1;36m%s\033[0m\n", packages_[pkg].url);
}

int levenshtein_dist(const char *s1, const char *s2) {
  unsigned int s1len, s2len, x, y, lastdiag, olddiag;
  s1len = strlen(s1);
  s2len = strlen(s2);
  unsigned int column[s1len + 1];
  for (y = 1; y <= s1len; y++)
    column[y] = y;
  for (x = 1; x <= s2len; x++) {
    column[0] = x;
    for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
      olddiag = column[y];
      column[y] = MIN3(column[y] + 1, column[y - 1] + 1,
                       lastdiag + (s1[y - 1] == s2[x - 1] ? 0 : 1));
      lastdiag = olddiag;
    }
  }
  return (column[s1len]);
}

void fuzz(const char *string, int *matches, unsigned n) {
  int *dists = (int *)malloc(sizeof(int) * n);
  for (unsigned i = 0; i < n; ++i) {
    dists[i] = -1;
  }
  for (unsigned i = 0; i < num_packages_; ++i) {
    int dist = levenshtein_dist(string, packages_[i].name);
    for (unsigned j = 0; j < n; ++j) {
      if (dists[j] == -1) {
        dists[j] = dist;
        matches[j] = i;
        break;
      } else if (dists[j] > dist) {
        for (unsigned k = n - 1; k > j; --k) {
          matches[k] = matches[k - 1];
          dists[k] = dists[k - 1];
        }
        matches[j] = i;
        dists[j] = dist;
        break;
      }
    }
  }
  free(dists);
}

int search(int argc, char *argv[]) {
  int count = 5;
  if (argv[2] == NULL) {
    printf("  \033[1;31mError: %s\033[0m\n", "Search requires a query");
    return -1;
  }
  if (argv[3] != NULL && is_digit(argv[3])) {
    count = atoi(argv[3]);
  }
  int *matches = (int *)malloc(sizeof(int) * count);
  for (unsigned i = 0; i < count; ++i) {
    matches[i] = -1;
  }
  fuzz(argv[2], matches, count);
  for (unsigned i = 0; i < count; ++i) {
    if (matches[i] != -1)
      print_package(matches[i], i + 1);
  }
  free(matches);
  return 0;
}

int main(int argc, char *argv[]) {
  if (NULL == argv[1] || 0 == strncmp(argv[1], "-h", 2) ||
      0 == strncmp(argv[1], "--help", 6)) {
    printf("%s\n", usage_);
    return 0;
  } else if (0 == strncmp(argv[1], "-v", 2) ||
             0 == strncmp(argv[1], "--version", 9)) {
    printf("%s\n", version_);
    return 0;
  } else if (0 == strncmp(argv[1], "i", 1) ||
             0 == strncmp(argv[1], "install", 7)) {
    return 1;
  } else if (0 == strncmp(argv[1], "s", 1) ||
             0 == strncmp(argv[1], "search", 6)) {
    return search(argc, argv);
  } else if (0 == strncmp(argv[1], "help", 4)) {
    return 5;
  }
  return 0;
}
