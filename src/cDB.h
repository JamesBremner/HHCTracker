
class cDB
{
    /// @brief The different attributes in database
    enum class eAttribute
    {
        none,
        role,
        name,
        nurselicence,
        inService,
    };

    /// @brief An attribute value for a person
    struct sValue
    {
        eAttribute aid;    /// attribute
        int pid;           /// person ID
        std::string value; /// value of attribute
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
        const std::string &name);

    vperson_t nursebyDate( int att);
    person_t nurse(int pid);
    vperson_t patient();
    person_t patient(int pid);

private:
    static int lastPID;
    std::vector<sValue> myValue; /// the database
    vperson_t myNurseList;       /// the displayed nurses
    vperson_t myPatientList;     /// the displayed patients

    /// @brief sort person list into attribute date order
    /// @param[in/out] list 
    /// @param att attribute index in person_t
    void sort(
        vperson_t& list,
        int att    );
};
