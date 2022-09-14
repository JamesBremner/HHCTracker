
class cDB
{
    /// @brief The different attributes in database
    enum class eAttribute
    {
        none,
        role,
        name,
        nurselicence,
        service,
        expire,
        certification,
        authorization,
        supplies,
    };

    /// @brief An attribute value for a person
    struct sValue
    {
        eAttribute aid;    /// attribute
        int pid;           /// person ID
        std::string value; /// value of attribute
        std::string text_for_file();
        void text_from_file();
    };

    /// @brief A pair describing a person
    /// first is the person's id
    /// second is a vector of strings containing the person's attributes
    typedef std::pair<
        int, std::vector<std::string>>
        person_t;

    /// @brief A vector describing a subset of persons
    typedef std::vector<person_t> vperson_t;

public:
    cDB();

    void addNurse(
        const std::string &name,
        const std::string &licence,
        const std::string &service);
    void updateNurse(
        int pid,
        const std::string &name,
        const std::string &licence,
        const std::string &service);
    void addPatient(
        const std::vector<std::string> &vals);

    /// @brief list of nurses for display, sorted by date
    /// @param[in] att index of attribute to sort by
    /// @return list
    vperson_t nursebyDate(int att);

    /// @brief nurse from display list
    /// @param[in] listIndex index
    /// @return description
    person_t nurselist(int listIndex);

    /// @brief nurse with pid
    /// @param[in] pid id
    /// @return description
    person_t nursePID(int pid);

    vperson_t patientbyDate(int att);

    person_t patientlist(int listIndex);
    person_t patientPID(int pid);

    void updatePatient(
        int pid,
        const std::vector<std::string>& vals     );

private:
    std::string myfname;
    static int lastPID;
    std::vector<sValue> myValue; /// the database
    vperson_t myNurseList;       /// the displayed nurses
    vperson_t myPatientList;     /// the displayed patients

    /// @brief sort person list into attribute date order
    /// @param[in/out] list
    /// @param att attribute index in person_t
    void sort(
        vperson_t &list,
        int att);
    void save();
    void load();
    long long  secs(
        const person_t& p,
        int index);
};
