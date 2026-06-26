// MenuModel.hpp -- YOUR submission for PA 0.  (Header-only.)
//
// Build a MenuModel that reads a .menu file (and optionally a .chosen file) and
// answers a few questions about it. The provided driver.cpp calls the public
// methods below, so keep their names and signatures exactly as given.
//
// RULES
//   * Header-only: put everything in this file. Do NOT modify driver.cpp.
//   * Standard library is allowed ONLY for input: <fstream>, <string>,
//   <stdexcept>, <utility>.
//   * Store the model's data in raw, dynamically-allocated arrays (new[] /
//   delete[]).
//     Do NOT use std::vector / std::map / other containers -- this assignment
//     is about managing memory yourself.
//   * Private data members use a LEADING underscore (e.g. _scope), as shown.
//   * Because your classes own raw memory, give them a correct rule of five
//     (destructor, copy constructor, move constructor, copy/move assignment).
//
// Look for the  // TODO  markers; the parts without a TODO are done for you.
#ifndef MENU_MODEL_HPP
#define MENU_MODEL_HPP

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility> // std::swap

// One course in the menu: its index (its position 0, 1, 2, ...) and how many
// dishes it offers.
struct Course {
    int courseIdx;
    int numDishes;
};

// One course chosen in advance (read from the .chosen file): which course,
// which dish.
struct ChosenCourse {
    int courseIdx;
    int dish;
};

// ---------------------------------------------------------------------------
// A pairing table: the courses it covers (its "scope") and its table of scores.
// It OWNS two dynamically-allocated arrays.
// ---------------------------------------------------------------------------
class PairingFactor {
  private:
    int _scopeSize;
    Course const **_scope; // pointers to the courses this table covers (owned
                           // by MenuModel)
    long long _numEntries;
    double *_scores;

  public:
    PairingFactor()
        : _scopeSize(0), _scope(nullptr), _numEntries(0), _scores(nullptr) {}

    // --- two-step construction (the file lists all scopes first, then all
    // score tables) ---
    void allocateScope(int scopeSize) {
        // TODO: remember the size in _scopeSize, and allocate _scope = new
        // Course const*[scopeSize].
        _scopeSize = scopeSize;
        _scope = new Course const *[scopeSize];
        // Scope is the amount of courses the that pairing table covers
    }
    void setCourse(int index, Course const *course) { _scope[index] = course; }

    void allocateScores(long long numEntries) {
        // TODO: remember the size in _numEntries, and allocate _scores = new
        // double[numEntries].
        _numEntries = numEntries;
        _scores = new double[numEntries];
    }
    void setScore(long long index, double score) { _scores[index] = score; }

    // --- rule of five (this object owns the _scope and _scores ARRAYS, not the
    // Courses) ---
    ~PairingFactor() {
        // TODO: delete[] the two arrays you allocated. (Deleting _scope frees
        // the array of
        //       pointers only -- the Courses they point at belong to MenuModel,
        //       leave them alone.)

        int i{0};
        double *score = _scores;
        for (; i < _numEntries; ++i, ++score) {
            delete score;
        }
        delete[] _scores;
    }

    PairingFactor(PairingFactor const &other)
        : _scopeSize(0), _scope(nullptr), _numEntries(0), _scores(nullptr) {
        // TODO: make this a DEEP copy -- allocate our own _scope (an array of
        // Course const*) and
        //       _scores, then copy each element from other (copy the scope
        //       POINTERS as they are; MenuModel's copy constructor re-points
        //       them into its own Courses).

        allocateScope(_scopeSize);
        Course const **other_course{other._scope};
        for (int i{0}; i < _scopeSize; ++i, ++other_course) {
            // Note: heap allocated array of heap allocated things.
            // Dereferencing gives pointer to heap allocated thing.
            _scope[i] = *other_course;
        }

        // Deep copy have to be performed for the score list
        allocateScores(other._numEntries);
        int i{0};
        double *score = other._scores;
        for (; i < _numEntries; ++i, ++score) {
            _scores[i] = *score;
        }
    }

    // Provided for you: swap, move constructor, and assignment (the
    // "copy-and-swap" idiom).
    friend void swap(PairingFactor &a, PairingFactor &b) noexcept {
        std::swap(a._scopeSize, b._scopeSize);
        std::swap(a._scope, b._scope);
        std::swap(a._numEntries, b._numEntries);
        std::swap(a._scores, b._scores);
    }
    PairingFactor(PairingFactor &&other) noexcept : PairingFactor() {
        swap(*this, other);
    }
    PairingFactor &operator=(PairingFactor other) {
        swap(*this, other);
        return *this;
    }

    // --- read-only accessors (used by the driver) ---
    int scopeSize() const { return _scopeSize; }
    int courseAt(int index) const { return _scope[index]->courseIdx; }
    long long numEntries() const { return _numEntries; }
    double scoreAt(long long index) const { return _scores[index]; }
};

// ---------------------------------------------------------------------------
// The whole menu-design model. It OWNS several dynamically-allocated arrays.
// ---------------------------------------------------------------------------
class MenuModel {
  private:
    int _numCourses;
    Course *_courses; // one Course per course: its index + dish count (length
                      // _numCourses)
    int _numFactors;
    PairingFactor *_factors;
    int _numChosen;
    ChosenCourse *_chosen;

  public:
    MenuModel()
        : _numCourses(0), _courses(nullptr), _numFactors(0), _factors(nullptr),
          _numChosen(0), _chosen(nullptr) {}

    explicit MenuModel(std::string const &menuPath) : MenuModel() {
        loadMenu(menuPath);
    }

    // --- rule of five ---
    ~MenuModel() {
        // TODO: give back the three arrays you allocated (_courses, _factors,
        // _chosen) with delete[].
        delete[] _courses;
        delete[] _factors;
        delete[] _chosen;
    }

    MenuModel(MenuModel const &other)
        : _numCourses(other._numCourses), _courses(new Course[_numCourses]),
          _numFactors(other._numFactors),
          _factors(new PairingFactor[_numFactors]),
          _numChosen(other._numChosen), _chosen(new ChosenCourse[_numChosen]) {
        // TODO: make this a DEEP copy of other (copy _courses, _factors, and
        // _chosen element by
        //       element into freshly-allocated arrays). IMPORTANT: after
        //       copying a factor, re-point its scope into THIS model's
        //       _courses, e.g. for each j: _factors[i].setCourse(j,
        //       &_courses[_factors[i].courseAt(j)]); (otherwise the copy's
        //       factors would still point into the other model's Courses).

        const Course *other_courses{other._courses};
        for (int i{0}; i < _numCourses; ++i) {
            _courses[i].courseIdx = other_courses[i].courseIdx;
            _courses[i].numDishes = other_courses[i].numDishes;
        }

        const PairingFactor *other_factors{other._factors};
        for (int i{0}; i < _numFactors; ++i) {
            _factors[i] = PairingFactor(other_factors[i]);
        }

        const ChosenCourse *other_chosen_courses{other._chosen};
        for (int i{0}; i < _numChosen; ++i) {
            _chosen[i].courseIdx = other_chosen_courses[i].courseIdx;
            _chosen[i].dish = other_chosen_courses[i].dish;
        }
    }

    friend void swap(MenuModel &a, MenuModel &b) noexcept {
        std::swap(a._numCourses, b._numCourses);
        std::swap(a._courses, b._courses);
        std::swap(a._numFactors, b._numFactors);
        std::swap(a._factors, b._factors);
        std::swap(a._numChosen, b._numChosen);
        std::swap(a._chosen, b._chosen);
    }
    MenuModel(MenuModel &&other) noexcept : MenuModel() { swap(*this, other); }
    MenuModel &operator=(MenuModel other) {
        swap(*this, other);
        return *this;
    }

    // Read the .chosen file and RECORD the chosen courses (no other processing
    // needed).
    void readChosen(std::string const &chosenPath) {
        // TODO: open the file; read the count k; allocate _chosen = new
        // ChosenCourse[k];
        //       read k pairs (courseIdx dish) into it; set _numChosen = k.

        // I'm gonna assume I have to handle if _chosen has already been
        // allocated

        if (chosenPath == "")
            return;
        if (_chosen) {
            delete[] _chosen;
        }

        std::ifstream in{chosenPath};
        if (!in.is_open()) {
            throw std::runtime_error("Invalid ChosenPath");
        }

        // First should be the number of courses
        int k;
        in >> k;

        _numChosen = k;
        _chosen = new ChosenCourse[k];

        int courseIdx;
        int dish;

        // I assume that formatting of .chosen is always correct
        for (int i{0}; in >> courseIdx; ++i) {
            // They have to be paired up
            in >> dish;
            _chosen[i].courseIdx = courseIdx;
            _chosen[i].dish = dish;
        }
    }

    // --- analyses (used by the driver) ---
    int numCourses() const { return _numCourses; }
    int dishes(int courseIdx) const { return _courses[courseIdx].numDishes; }
    int numTables() const { return _numFactors; }
    long long completeMeals() const {
        // TODO: return the product of every course's dish count
        //       (_courses[0].numDishes * _courses[1].numDishes * ...).

        long long product{_courses[0].numDishes};

        for (int i{1}; i < _numCourses; ++i) {
            product *= _courses[i].numDishes;
        }

        return product;
    }
    int chosenCount() const { return _numChosen; }
    int freeCourses() const { return _numCourses - _numChosen; }
    int chosenCourseAt(int i) const { return _chosen[i].courseIdx; }
    int chosenDishAt(int i) const { return _chosen[i].dish; }

    // --- forward iterator over the pairing tables (provided) ---
    class Iterator {
      public:
        explicit Iterator(PairingFactor const *ptr) : _ptr(ptr) {}
        PairingFactor const &operator*() const { return *_ptr; }
        Iterator &operator++() {
            ++_ptr;
            return *this;
        }
        bool operator!=(const Iterator &other) const {
            return _ptr != other._ptr;
        }

      private:
        PairingFactor const *_ptr;
    };
    Iterator begin() const { return Iterator(_factors); }
    Iterator end() const { return Iterator(_factors + _numFactors); }

  private:
    void loadMenu(std::string const &menuPath) {
        // TODO: open menuPath (throw std::runtime_error if it won't open) and
        // read:
        //   SECTION 1 (the tag): the word "MENU" -- read it and ignore it
        //   SECTION 2 (the courses): the number of courses -> _numCourses;
        //   allocate
        //      _courses = new Course[_numCourses]; for each i set
        //      _courses[i].courseIdx = i and read _courses[i].numDishes
        //   SECTION 3 (the pairing scopes): the number of tables ->
        //   _numFactors; allocate _factors;
        //      then for each table read its scope size and each course index,
        //      pointing the factor at the stored Course with setCourse(j,
        //      &_courses[idx]) (allocateScope first)
        //   SECTION 4 (the pairing scores): for each table read its entry count
        //   then that many
        //      scores (use allocateScores / setScore on _factors[i])

        std::ifstream menuFile{menuPath};
        if (!menuFile.is_open()) {
            throw std::runtime_error("Invalid menu path");
        }

        // Ignore "MENU" header
        std::string ignored;
        menuFile >> ignored;

        // Extract number of courses
        menuFile >> _numCourses;

        _courses = new Course[_numCourses];
        int numDishes;

        // Finishing Section 2
        for (int i{0}; i < _numCourses; ++i) {
            menuFile >> numDishes;
            _courses[i].courseIdx = i;
            _courses[i].numDishes = numDishes;
        }

        // Section 3: Pairing tables
        std::string configLine;
        menuFile >> _numFactors;

        _factors = new PairingFactor[_numFactors];

        for (int i{0}; i < _numFactors; ++i) {
            // "1 0" => One course assigned, assign course 0.
            // "2 1 3" => Two courses assigned, assign course 3.
            int coursesNum;
            menuFile >> coursesNum;
            _factors[i].allocateScope(coursesNum);

            for (int j{0}; j < coursesNum; ++j) {
                int courseIdx;
                menuFile >> courseIdx;
                _factors[i].setCourse(i, &_courses[courseIdx]);
            }
        }

        // Section 4: Assigning Scores
    }
};

#endif // MENU_MODEL_HPP
