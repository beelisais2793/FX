/*
 ==============================================================================

 This file is part of the RT-WDF library.
 Copyright (c) 2015,2016 - Maximilian Rest, Ross Dunkel, Kurt Werner.

 Permission is granted to use this software under the terms of either:
 a) the GPL v2 (or any later version)
 b) the Affero GPL v3

 Details of these licenses can be found at: www.gnu.org/licenses

 RT-WDF is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 -----------------------------------------------------------------------------
 To release a closed-source product which uses RT-WDF, commercial licenses are
 available: write to rt-wdf@e-rm.de for more information.

 ==============================================================================

 rt-wdf.h
 Created: 26 Oct 2015 11:00:00am
 Author:  mrest

 ==============================================================================
 */

#pragma once

//==============================================================================
#include <memory>
#include <armadillo>
#include <assert.h>
using namespace arma;


//==============================================================================
/** A struct that holds matrices for R-type and NL root nodes.

    wdfRootRtype only uses the S matrix of this struct.
    wdfRootNL only uses the E,F,M,N matrices of this struct.

    @see wdfRootRtype, wdfRootNL
*/
typedef struct matData{

    /** S-Matrix as used in wdfRootRtype.
        Size: (numBrPorts) x (numBrPorts)
    */
    mat Smat;

    /** E-Matrix as used in wdfRootNL.
        Size: (numNlPorts) x (numBrPorts)
    */
    mat Emat;

    /** F-Matrix as used in wdfRootNL.
        Size: (numNlPorts) x (numNlPorts)
    */
    mat Fmat;

    /** M-Matrix as used in wdfRootNL.
        Size: (numBrPorts) x (numBrPorts)
    */
    mat Mmat;

    /** N-Matrix as used in wdfRootNL.
        Size: (numBrPorts) x (numNlPorts)
    */
    mat Nmat;

    /** T-Matrix as used in wdfRootLinear and wdfRootMixed.
     Size: (numBrPorts) x (numNlPorts)
     */
    mat Tmat;

} matData;

typedef enum paramType {
    boolParam,
    doubleParam
} paramType;

typedef struct paramData {
    std::string name;
    size_t ID;
    paramType type;
    DspFloatType value;
    std::string units;
    DspFloatType lowLim;
    DspFloatType highLim;
} paramData;

// Diodes:
/** Enum to specify single diode model in wdfRootNL */
#define DIODE       00
/** Enum to specify anti-parallel diode pair model in wdfRootNL */
#define DIODE_AP    01

// Bipolar Transistors:
/** Enum to specify Ebers-Moll bipolar npn-transistor model in wdfRootNL */
#define NPN_EM      10

// Triode Tubes:
/** Enum to specify Dempwolf triode tube model in wdfRootNL */
#define TRI_DW      20



//==============================================================================
// Forward declarations
class nlModel;
class diodeModel;
class diodeApModel;
class bjtEmModel;
class triDwModel;



//==============================================================================
class nlModel {

public:
    //----------------------------------------------------------------------
    /**
     Parent class for all non-linear models.

     @param numPorts            defines the number of ports that are used to
                                implement a specific NL model.
    */
    nlModel( int numPorts );

    //----------------------------------------------------------------------
    /**
     Deconstructor.
    */
    virtual ~nlModel( );

    //----------------------------------------------------------------------
    /**
     Virtual function to calculate the non-linear current(s) fNL and
     Jacobian JNL according to a vector of input voltage(s).

     @param *fNL             is a pointer to store result of fNL(x).
                             This is a current.
     @param *JNL             is a pointer to store the result of the
                             Jacobian of fNL(x).
     @param *x               is a pointer to read the input value x.
                             This is a voltage.
     @param *currentPort     is a pointer to the first reading /
                             writing position in fNL, JNL and x.
    */
    virtual void calculate( vec* fNL,
                            mat* JNL,
                            vec* x,
                            int* currentPort ) = 0;

    //----------------------------------------------------------------------
    /**
     Function which returns the number of ports on an NL model for memory house-
     keeping

     @returns                   the number of ports implemented in a particular
                                NL model.
    */
    int getNumPorts( );

private:
    //----------------------------------------------------------------------
    /** Stores the number of ports of a model */
    unsigned int numPorts;

};


//==============================================================================
class diodeModel : public nlModel {

public:
    //----------------------------------------------------------------------
    /**
     Single diode model class.

     Single Diode Model according to Kurt Werner et al
     ("An Improved and Generalized Diode Clipper Model for Wave Digital Filters")
    */
    diodeModel( );

    //----------------------------------------------------------------------
    /**
     Calculate the non-linear current and it's Jacobian according to an
     input voltage.

     @param *fNL             is a pointer to store result of fNL(x).
                             This is a current.
     @param *JNL             is a pointer to store the result of the
                             Jacobian of fNL(x).
     @param *x               is a pointer to read the input value x.
                             This is a voltage.
     @param *currentPort     is a pointer to the first reading /
                             writing position in fNL, JNL and x.
    */
    void calculate( vec* fNL,
                    mat* JNL,
                    vec* x,
                    int* currentPort );

};


//==============================================================================
class diodeApModel : public nlModel {

public:
    //----------------------------------------------------------------------
    /**
     Anti-parallel diode pair model class.

     Anti-Parallel Diode Model according to Kurt Werner et al
     ("An Improved and Generalized Diode Clipper Model for Wave Digital Filters")
    */
    diodeApModel();

    //----------------------------------------------------------------------
    /**
     Calculate the non-linear current and it's Jacobian according to an
     input voltage.

     @param *fNL             is a pointer to store result of fNL(x).
                             This is a current.
     @param *JNL             is a pointer to store the result of the
                             Jacobian of fNL(x).
     @param *x               is a pointer to read the input value x.
                             This is a voltage.
     @param *currentPort     is a pointer to the first reading /
                             writing position in fNL, JNL and x.
    */
    void calculate( vec* fNL,
                    mat* JNL,
                    vec* x,
                    int* currentPort );

};


//==============================================================================
class npnEmModel : public nlModel {

public:
    //----------------------------------------------------------------------
    /**
     Ebers-Moll npn-transistor model class.

     Transistor Models using Ebers-Moll equations
     ("Large-signal behavior of junction transistors")
    */
    npnEmModel();

    //----------------------------------------------------------------------
    /**
     Calculate the non-linear current and it's Jacobian according to an
     input voltage.

     @param *fNL             is a pointer to store result of fNL(x).
                             This is a current.
     @param *JNL             is a pointer to store the result of the
                             Jacobian of fNL(x).
     @param *x               is a pointer to read the input value x.
                             This is a voltage.
     @param *currentPort     is a pointer to the first reading /
                             writing position in fNL, JNL and x.
    */
    void calculate( vec* fNL,
                    mat* JNL,
                    vec* x,
                    int* currentPort );

};


//==============================================================================
class triDwModel : public nlModel {

public:
    //----------------------------------------------------------------------
    /**
     Triode tube model class.

     Triode model according to Dempwolf et al
     ("A physically-motivated triode model for circuit simulations")
    */
    triDwModel();

    //----------------------------------------------------------------------
    /**
     Calculate the non-linear current and it's Jacobian according to an
     input voltage.

     @param *fNL             is a pointer to store result of fNL(x).
                             This is a current.
     @param *JNL             is a pointer to store the result of the
                             Jacobian of fNL(x).
     @param *x               is a pointer to read the input value x.
                             This is a voltage.
     @param *currentPort     is a pointer to the first reading /
                             writing position in fNL, JNL and x.
    */
    void calculate( vec* fNL,
                    mat* JNL,
                    vec* x,
                    int* currentPort );

};

//==============================================================================
// Define enums for solver identifiers


// Iterative:
// TODO: introduce enums!
/** Enum to specify a Newton Solver*/
#define NEWTON_SOLVER   1



//==============================================================================
// Newton Solver config parameters

/** tolerance for ||F||_2 */
#define TOL     1.0e-06                     // TODO: evaluate physically meaningful tolerance.
/** limit on function evaluations */
#define ITMAX   50


//==============================================================================
// Forward declarations
class nlSolver;
class nlNewtonSolver;


//==============================================================================
class nlSolver {

public:
    //----------------------------------------------------------------------
    /**
     Parent class for all non-linear solvers.
    */
    nlSolver();

    //----------------------------------------------------------------------
    /**
     Deconstructor.
    */
    virtual ~nlSolver();

    //----------------------------------------------------------------------
    /**
     Function which returns the number of ports on an NL model for memory house-
     keeping

     @returns                   the total number of non-linear ports which are
                                present at the root.
    */
    int getNumPorts();

    //----------------------------------------------------------------------
    /**
     Virtual function that processes a vector of incoming waves and
     returns a vector of outgoing waves according to the specified
     nonlinearities.

     Must be implemented in an NL solver according to the desired behaviour.

     @param *inWaves            is a pointer to a vector of incoming waves
     @param *outWaves           is a pointer to a vector of outgoing waves
     @param *myMatData          is a pointer to the E,F,M,N (and S) matrices
    */
    virtual void nlSolve( vec* inWaves,
                          vec* outWaves ) = 0;

    //----------------------------------------------------------------------
    /**
     Vector of enums that specify the types on non-linearities in the solver
    */
    std::vector<nlModel*> nlModels;
    /**
     Total number of non-linear ports which are present at the root.
    */
    int numNLPorts;

};


//==============================================================================
class nlNewtonSolver : public nlSolver {

protected:
    //----------------------------------------------------------------------
    /** struct which holds all root NLSS matrices including variable conversion */
    matData* myMatData;
    /** latest guess to solve the NLSS system */
    vec* x0;
    /** result of the NL equations */
    vec* fNL;
    /** Jacobian of the NL equations */
    mat* JNL;
    /** F vector for newton method */
    vec* F;
    /** J matrix for the newton method */
    mat* J;
    /** variable to store Fmat * fNL for x0 next prediction */
    vec* Fmat_fNL;
    /** flag to detect first run of the solver for a clean first initial guess */
    bool firstRun = true;

public:
    //----------------------------------------------------------------------
    /**
     Multi-dimensional Newton Solver class.

     Newton solver as introduced by Michael Jorgen Olsen:
     ("Resolving grouped nonlinearities in wave digital filters using
     iterative techniques"), DAFx-16

     Creates a newton solver and it's nonlinearities

     @param nlList              is a vector of enums that specify the types of
                                nonlinearities
     @param *myMatData          is a pointer to the E,F,M,N (and S) matrices
    */
    nlNewtonSolver( std::vector<int> nlList,
                matData* myMatData );

    /**
     Deconstructor.
    */
    ~nlNewtonSolver( );

    //----------------------------------------------------------------------
    /**
     Solver function that processes a vector of incoming waves and
     returns a vector of outgoing waves according to the specified
     nonlinearities.

     The actual solver operates on J and F matrices which are calculated
     by evalNlModels based on fNL, JNL, Emat and Fmat.

     The result outWaves is calculated based on inWaves, x, Nmat, Mmat.

     @param inWaves             is a pointer to a vector of incoming waves
     @param outWaves            is a pointer to a vector of outgoing waves
    */
    void nlSolve( vec* inWaves,
                  vec* outWaves );

    //----------------------------------------------------------------------
    /**
     Evaluates all non-linear model members of a solver and sets J and F
     members properly.

     It calculates J and F matrices based on both fNL, JNL as returned
     from nlModel.calculate(x,..) and Emat, Fmat from myMatData.

     @param *inWaves            is a pointer to a vector of incoming waves
     @param *myMatData          is a pointer to the E,F,M,N matrices
     @param *x                  is a pointer to the input values x
    */
    void evalNlModels(vec* inWaves, matData* myMatData, vec* x);

};



//==============================================================================
// Forward declarations

// WDF TREE
class wdfTree;

// ROOTS:
class wdfRoot;
    class wdfRootRtype;
    class wdfRootNL;
    class wdfRootSimple;

// UTILITIES:
class wdfPort;

// WDF TREE ELEMENTS
class wdfTreeNode;
    class wdfTerminatedAdapter;
        class wdfTerminatedRtype;
        class wdfTerminatedSeries;
        class wdfTerminatedParallel;
        class wdfInverter;
    class wdfTerminatedLeaf;
        class wdfTerminatedCap;
        class wdfTerminatedInd;
        class wdfTerminatedRes;
        class wdfTerminatedResVSource;

// WDF ROOT ELEMENTS
class wdfRootNode;
    class wdfUnterminatedSwitch;
    class wdfUnterminatedCap;
    class wdfUnterminatedInd;
    class wdfUnterminatedRes;
    class wdfIdealVSource;
    class wdfIdealCSource;


#pragma mark - Tree
//==============================================================================
//                                  T R E E
//==============================================================================
class wdfTree {

public:
    //----------------------------------------------------------------------
    /**
     Base class for a WDF tree.

     This class is always user-extended by a subclass that specifies the
     implementation of a WDF structure. It provides a standard interface for
     initialization and operation of a WDF structure towards the owner of
     the tree object.
     */
    wdfTree( );

    //----------------------------------------------------------------------
    /**
     Deconstructor
     */
    virtual ~wdfTree( );

protected:
    //----------------------------------------------------------------------
    /**
     Pointer to the root object of this tree.
     */
    std::unique_ptr<wdfRoot> root;

    //----------------------------------------------------------------------
    /**
     Array of pointers to the first nodes in the subtrees that hang off
     the root.
     */
    wdfTreeNode **subtreeEntryNodes;

    //----------------------------------------------------------------------
    /**
     Pointer to a vector of ascending waves towards the root.
     */
    std::unique_ptr<vec> ascendingWaves;

    //----------------------------------------------------------------------
    /**
     Pointer to a vector of descending waves from the root.
     */
    std::unique_ptr<vec> descendingWaves;

    //----------------------------------------------------------------------
    /**
     Samplerate of the tree.

     Must be set correctly in the user-specific wdfTree application by
     setSamplerate()
     */
    DspFloatType treeSampleRate;

    //----------------------------------------------------------------------
    /**
     Number of subtrees
     */
    size_t subtreeCount;

    //----------------------------------------------------------------------
    /**
      Vector of port resistances at the root
     */
    DspFloatType *Rp;

    //----------------------------------------------------------------------
    /**
      Vector of parameters.

      Parameters must be defined in the user-specific wdfTree extension as
      paramData objects and added to this vector.
     */
    std::vector<paramData> params;

public:
    //----------------------------------------------------------------------
    /**
     High level function to initialize all subtrees and the root.

     Sets parents in all children in subtrees and creates ports in their nodes.
     */
    void initTree( );

    //----------------------------------------------------------------------
    /**
     Function to set the sample rate.

     Samplerate needs to be set before adaptTree() is called!

     @param fs                  samplerate to use for adaptation of reactive
                                elements.
     */
    void setSamplerate( DspFloatType fs );

    //----------------------------------------------------------------------
    /**
     Function to get the sample rate.

     @returns                   a DspFloatType with the tree's sample rate
     */
    DspFloatType getSamplerate( );

    //----------------------------------------------------------------------
    /**
     High level function to adapt all ports in subtrees.

     Recursively adapts all subtrees towards the root according to sampleRate.
     Updates root matrix data afterwards if applicable to take
     new port resistances into account.
     */
    int adaptTree( );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to update a root's matrix
     element according to the port resistances of the subtrees.

     This function gets a roots' matData* pointer and inits/updates
     it's elements accordingly if applicable for this subtype of root.

     Must be implemented in the user specific wdfTree extension.

     @param rootMatrixData      is a pointer to the matData object of the
                                root
     @param Rp                  is a vector of port resistances of all
                                subtrees of the root.

     @returns                   0 for success, -1 for error
     */
    virtual int setRootMatrData( matData* rootMatrixData,
                                 DspFloatType *Rp ) = 0;

    //----------------------------------------------------------------------
    /**
     High level function that is called to evaluate the WDF structure for
     one sample.

     It pulls the waves from all connected subtrees, passes them to the
     root and finally pushes the roots' answer down back into the subtrees.
     */
    void cycleWave( );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to set the circuit's input
     value at the correct source.

     Must be implemented in the user specific wdfTree extension.

     @param signalIn            voltage or current of the input source
     */
    virtual void setInputValue( DspFloatType signalIn ) = 0;

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to get the circuit's output
     value.

     Must be implemented in the user specific wdfTree extension.

     @returns                   a DspFloatType type output value
     */
    virtual DspFloatType getOutputValue( ) = 0;

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to return an identifier string
     for the tree.

     Must be implemented in the user specific wdfTree extension.

     @returns                   a const char* as an identifier string for the
                                tree
     */
    virtual const char* getTreeIdentifier( ) = 0;

    //----------------------------------------------------------------------
    /**
     Function that returns a reference to the tree's param data vector.

     @returns                   a reference to the tree's param data vector
     */
    const std::vector<paramData>& getParams( );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to set a specific parameter to
     a new value.

     Must be implemented in the user specific wdfTree extension.

     @param paramID             parameter ID as specified by the user in wdfTree
                                application class.
     @param paramValue          value to set the parameter to.
     */
    virtual void setParam( size_t paramID,
                           DspFloatType paramValue ) = 0;

};

#pragma mark - Roots
//==============================================================================
//                                 R O O T S
//==============================================================================
class wdfRoot {

public:
    //----------------------------------------------------------------------
    /**
     Base class for all root subclasses.
     */
    wdfRoot( );

    //----------------------------------------------------------------------
    /**
     Deconstructor
     */
    virtual ~wdfRoot( );

    //----------------------------------------------------------------------
    /**
     Function that does nothing if not overwritten by a method in a subclass.

     Acts as an interface towards the tree to do anything with the subtree's
     upfacing port resistances in the root. Is called during adaptation of
     the WDF structure.

     @param Rp                  is a vector of port resistances of all
                                subtrees of the root.
     */
    virtual void setPortResistances( DspFloatType * Rp );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to process a vector of ascending
     waves in the root and return the result.

     Acts a an interface to the root for wdfTree and it's cycleWave()
     function.

     @param *ascendingWaves     is a pointer to a vector of waves that were
                                collected at the upfacing ports of the subtrees,
                                traveling into the root.
     @param *descendingWaves    is a pointer to a vector to store the resulting
                                waves that leave the root afterprocessing into
                                the subtrees
     */
    virtual void processAscendingWaves( vec* ascendingWaves,
                                        vec* descendingWaves ) = 0;

    //----------------------------------------------------------------------
    /**
     Virtual function that may return a pointer to a root's rootMatrixData
     element.

     Returns a valid pointer or NULL if not applicable for the
     specific root subclass. The valid pointer is used to populate or
     update a root's matrix coefficients by setRootMatrData( ) of the tree.

     @returns                   matData* to the rootMatrixData element
     */
    virtual matData* getRootMatrPtr( );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to return a String
     describing the type of this root.

     @returns                   a String describing the type of this root
     */
    virtual std::string getType( ) const = 0;

};

//==============================================================================
class wdfRootRtype : public wdfRoot {

private:
    //----------------------------------------------------------------------
    /**
     Pointer to an object which holds all matrices of coefficients of the
     root.
     */
    std::unique_ptr<matData> rootMatrixData;

    //----------------------------------------------------------------------
    /**
     Number of subtrees that are connected to the root.
     */
    int numSubtrees;

public:
    //----------------------------------------------------------------------
    /**
     R-type root implementation.

     @param numSubtrees         specifies the number of subtrees which are
                                connected to the root to correctly allocate
                                memory for rootMatrixData
     */
    wdfRootRtype( int numSubtrees );

    //----------------------------------------------------------------------
    /**
     Deconstructor
     */
    ~wdfRootRtype( );

    //----------------------------------------------------------------------
    /**
     Function to process a vector of ascending waves in the root and return
     the descending result.

     Scatters the incoming waves according to the S-Matrix of rootMatrixData.

     @param *ascendingWaves     is a pointer to a vector of waves that were
                                collected at the upfacing ports of the subtrees,
                                traveling into the root.
     @param *descendingWaves    is a pointer to a vector to store the resulting
                                waves that leave the root afterprocessing into
                                the subtrees
     */
    virtual void processAscendingWaves( vec* ascendingWaves,
                                        vec* descendingWaves );

    //----------------------------------------------------------------------
    /**
     Function that returns a pointer to the rootMatrixData element.

     The pointer is be used to populate or update the S-Matrix of the
     R-type adapter by the setRootMatrData() function of the tree.

     @returns                   matData* to the rootMatrixData element
     */
    virtual matData* getRootMatrPtr( );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root.

     @returns                   a String describing the type of this root as
                                "Root (R-type)"
     */
    virtual std::string getType( ) const;

};



//==============================================================================
class wdfRootNL : public wdfRoot {

private:
    //----------------------------------------------------------------------
    /**
     Pointer to an object which holds all matrices of coefficients of the
     root.
     */
    std::unique_ptr<matData> rootMatrixData;

    //----------------------------------------------------------------------
    /**
     Number of subtrees which are connected to the root.
     */
    int numSubtrees;

    //----------------------------------------------------------------------
    /**
     Pointer to a NL Solver which solves the implicit scattering loop
     around the non-linearities in the root.
     */
    std::unique_ptr<nlNewtonSolver> NlSolver;

public:
    //----------------------------------------------------------------------
    /**
     Multiple/Multiport Non-Linearities root implementation.

     @param numSubtrees         specifies the number of subtrees which are
                                connected to the root to correctly allocate
                                memory for rootMatrixData
     @param nlList              is a vector defines that map to available
                                NL models. See nlModelClass.h for all models
     @param solverType          sets the solver to use in this root. See
                                nlSolverClass.h for all solvers.
     */
    wdfRootNL( int numSubtrees,
               std::vector<int> nlList,
               int solverType );

    //----------------------------------------------------------------------
    /**
     Deconstructor
     */
    ~wdfRootNL( );

    //----------------------------------------------------------------------
    /**
     Function to process a vector of ascending waves in the root and return
     the descending result.

     Scatters the waves according to the R-type connector towards the
     non-linearities, solves the inner loop with a NLSolver and returns
     the result.

     @param *ascendingWaves     is a pointer to a vector of waves that were
                                collected at the upfacing ports of the subtrees,
                                traveling into the root.
     @param *descendingWaves    is a pointer to a vector to store the resulting
                                waves that leave the root afterprocessing into
                                the subtrees
     */
    virtual void processAscendingWaves( vec* ascendingWaves,
                                        vec* descendingWaves );

    //----------------------------------------------------------------------
    /**
     Function that returns a pointer to the rootMatrixData element.

     The pointer is be used to populate or update the E,-F-,M-,N-Matrices
     of the root loop by the setRootMatrData() function of the tree.

     @returns                   matData* to the rootMatrixData element
     */
    virtual matData* getRootMatrPtr( );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root.

     @returns                   a String describing the type of this root as
                                "Root (NL-type)"
     */
    virtual std::string getType( ) const;

};



//==============================================================================
class wdfRootSimple : public wdfRoot {

private:
    //----------------------------------------------------------------------
    /**
     Pointer to a single unadapted root element.
     */
    wdfRootNode* rootElement;

public:
    //----------------------------------------------------------------------
    /**
     Single unadapted closed-form element root implementation.

     @param *rootElement         pointer

     */
    wdfRootSimple ( wdfRootNode* rootElement );

    //----------------------------------------------------------------------
    /**
     Sets correct port resistance from the subtrees in RootNode.

     @param Rp               is a vector of port resistances of all
     subtrees of the root.
     */
    void setPortResistances(DspFloatType * Rp);

    //----------------------------------------------------------------------
    /**
     Function to process a vector (of length 1) of the ascending wave
     towards a single unadapted closed-form root node.

     @param *ascendingWaves     is a pointer to a vector of waves that were
                                collected at the upfacing ports of the subtrees,
                                traveling into the root.
     @param *descendingWaves    is a pointer to a vector to store the resulting
                                waves that leave the root afterprocessing into
                                the subtrees
     */
    virtual void processAscendingWaves( vec* ascendingWaves,
                                        vec* descendingWaves );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root.

     @returns                   a String describing the type of this root  as
                                "Root (Simple-type)"
     */
    virtual std::string getType( ) const ;

};

# pragma mark - Basic Tree Classes
//==============================================================================
//                                  P O R T
//==============================================================================
/**
 Port class to describe a wave digital filter port.
 */
class wdfPort {

public:
    //----------------------------------------------------------------------
    /**
     Creates a WDF port on connectedNode.

     @param *connectedNode      is a pointer to the node that connects to that
                                port.
     */
    wdfPort( wdfTreeNode* connectedNode );

    //----------------------------------------------------------------------
    /**
     Calculates the port voltage based on wave variables a and b

     @returns                   port voltage in Volt
     */
    DspFloatType getPortVoltage( );

    //----------------------------------------------------------------------
    /**
     Calculates the port current based on wave variables a and b

     @returns                   port current in Ampere
     */
    DspFloatType getPortCurrent( );

    //----------------------------------------------------------------------
    /**
     The WDF port resistance in Ohm
     */
    DspFloatType Rp;

    //----------------------------------------------------------------------
    /**
     The inverse WDF port resistance in Siemens
     */
    DspFloatType Gp;

    //----------------------------------------------------------------------
    /**
     Incident wave (incoming wave)
     */
    DspFloatType a;

    //----------------------------------------------------------------------
    /**
     Reflected wave (outgoing wave)
     */
    DspFloatType b;

    //----------------------------------------------------------------------
    /**
     Pointer to the node that connects to that port.
     */
    wdfTreeNode* connectedNode;

};


//==============================================================================
//                             T R E E   N O D E
//==============================================================================
/**
 Tree Node class to describe a wave digital filter node in a subtree below
 the root
 */
class wdfTreeNode {

public:
    //----------------------------------------------------------------------
    /**
     Base class of a WDF Tree Node.

     This is the base class for all adapted WDF elements in subtrees with more
     than n=2 ports that hang off the root.

     @param *childrenNodes      holds a vector of pointers to all children
                                connected to this element in the tree
     */
    wdfTreeNode( std::vector<wdfTreeNode*> childrenIn );

    /**
     Base class of a WDF Tree Node.

     This version of the constructor is used as the base class for three port
     nodes in the tree (series and parallel adaptors)

     @param *left               holds a pointer to the left child of the node
     @param *right              holds a pointer to the right child of the node
     */
    wdfTreeNode( wdfTreeNode *left,
                 wdfTreeNode *right );

    /**
     Base class of a WDF Tree Node.

     This version of the constructor is used as the base class for all adapted
     WDF leafs.
     */
    wdfTreeNode( );

    //----------------------------------------------------------------------
    /**
     Destructor
     */
    virtual ~wdfTreeNode( ) {}

    //----------------------------------------------------------------------
    /**
     Recursively sets pointers to parent node in children.

     This recursion should be initiated from the base of the tree. It
     sets a pointer to *this parent node in all connected children.
     */
    void setParentInChildren( );

    //----------------------------------------------------------------------
    /**
     Recursively creates WDF ports in the tree.

     This recursion should be initiated from the base of the tree. It
     creates downfacing ports for all children and an upfacing port in
     each child while setting the correct pointers to the nodes that
     these ports are connected to.
     */
    void createPorts( );

    //----------------------------------------------------------------------
    /**
     Recursively adapts all ports in the tree.

     This recursion should be initiated from the base of the tree. It sets
     port resistances of all upfacing ports according to component values
     and/or adaptation rules and copies it onto connected downfacing ports
     in the parent.

     @param sampleRate          sample rate as specified by setSamplerate()
     @returns                   a DspFloatType type up-facing port resistance of that
                                WDF element
     */
    DspFloatType adaptPorts( DspFloatType sampleRate );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to return the nodes' upfacing
     port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of a tree node to fulfill adaptation according to
     the respective adaptation law of this node.

     @param sampleRate          sample rate as needed to adapt 
                                capacitors/inductors
     @returns                   a DspFloatType type port resistance of that element
                                in Ohms
     */
    virtual DspFloatType calculateUpRes( DspFloatType sampleRate ) = 0;

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to recursively calculate
     scattering coefficients of adapters.

     This recursion should be initiated from the base of the tree. It
     calculates the scattering coefficients according to the port
     resistances of an element.
     */
    virtual void calculateScatterCoeffs( ) = 0;

    //----------------------------------------------------------------------
    /**
     Recursively pulls the wave from the leafs towards the root.

     This recursion should be initiated from the base of the tree. As all
     tree nodes are adapted, it basically just pulls up correctly
     scattered states of reactive elements and input values from adapted
     sources.

     @returns                   the upward traveling wave of a node
     */
    virtual DspFloatType pullWaveUp( );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to return the upfacing wave
     component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base according to the element type.

     @returns                   the upward traveling wave of a node
     */
    virtual DspFloatType calculateUpB( ) = 0;

    //----------------------------------------------------------------------
    /**
     Recursively pushes the wave from the root towards the leafs.

     This recursion should be initiated from the base of the tree. It
     pushes the respective wave component of a branch from the root to
     the leafs by scattering them correctly along the way.

     @param  descendingWave     descending wave component that shall be
                                scattered to the downfacing ports of the node or
                                processed in a leaf accordingly
     */
    virtual void pushWaveDown( DspFloatType descendingWave );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to set the wave components in
     the downfacing ports.

     This function processes the input wave correctly for the downfacing
     ports and sets them in the port objects.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( DspFloatType descendingWave ) = 0;

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to return a String describing
     the type of this node.

     @returns                   a String describing the type of this root
     */
    virtual std::string getType( ) const = 0;

    //----------------------------------------------------------------------
    /**
     Pointer to the upfacing port object of this node.
     */
    std::unique_ptr<wdfPort> upPort;

    //----------------------------------------------------------------------
    /**
     Pointer to the connected parent node of this node.
     */
    wdfTreeNode* parentNode;

protected:
    //----------------------------------------------------------------------
    /**
     Vector of pointers to downfacing port objects of this tree.
     */
    std::vector<wdfPort*> downPorts;

    //----------------------------------------------------------------------
    /**
     Vector of pointers to connected children nodes of this node.
     */
    std::vector<wdfTreeNode*> childrenNodes;

};

#pragma mark - Terminated Adapters
//==============================================================================
//              T E R M I N A T E D   A D A P T E R S   (S, P, R)
//==============================================================================
class wdfTerminatedAdapter: public wdfTreeNode {

public:
    //----------------------------------------------------------------------
    /**
     Base class for terminated 2 port adapters in WDF subtrees/branches.

     This includes series and parallel adapters with one terminated
     upfacing port and 2 downfacing ports.

     @param left                pointer to the adapter's left children
     @param right               pointer to the adapter's right children
     */
    wdfTerminatedAdapter( wdfTreeNode *left,
                          wdfTreeNode *right );
    /**
     Base class for terminated n port adapters in WDF subtrees/branches.

     This includes r-type adapters with one terminated upfacing port and n
     downfacing ports.

     @param *childrenNodes      holds a vector of pointers to the n adapter's
                                children
     */
    wdfTerminatedAdapter( std::vector<wdfTreeNode*> childrenIn );


};

//==============================================================================
class wdfTerminatedRtype: public wdfTerminatedAdapter {

public:
    //----------------------------------------------------------------------
    /**
     Adapter class for terminated n-port R-type nodes.

     The adapter owns one terminated upfacing port and n downfacing ports

     @param *childrenNodes      holds a vector of pointers to the n adapters'
                                children
     */
    wdfTerminatedRtype( std::vector<wdfTreeNode*> childrenIn );

    //----------------------------------------------------------------------
    /**
     Destructor

     */
    ~wdfTerminatedRtype();

    //----------------------------------------------------------------------
    /**
     Returns the r-type adaptors upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the respective adaptation law of this node.

     Must be implemented in the user-defined subclass of wdfTerminatedRtype in
     the WDF application tree.

     @param sampleRate          sample rate as needed to adapt
                                capacitors/inductors
     @returns                   a DspFloatType type port resistance of that element
                                in Ohms
     */
    virtual DspFloatType calculateUpRes( DspFloatType sampleRate ) = 0;

    //----------------------------------------------------------------------
    /**
     Recursively calculates scattering coefficients of adapters.

     This recursion should be initiated from the base of the tree. It
     calculates the scattering coefficients according to the port resistances
     of the adaptor.

     Must be implemented in the user-defined subclass of wdfTerminatedRtype in
     the WDF application tree. Make sure to continue the recursion by calling
     downPort->connectedNode->calculateScatterCoeffs( ) on all down ports.
     */
    virtual void calculateScatterCoeffs( ) = 0;

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     It collects the wave components from the downfacing ports and weights them
     with the coefficients from the scattering matrix S.

     @returns                   the upward traveling wave of a node
     */
    virtual DspFloatType calculateUpB( );

    //----------------------------------------------------------------------
    /**
     Sets the wave components in the downfacing ports.

     This function weights the input wave and the incident waves on the
     downports with the coefficients in the scattering matrix S and sets the
     reflected waves in the downfacing port objects.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( DspFloatType descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this adaptor.

     @returns                   a String describing the type of this adaptor as
                                "R-type Adapter (TOP adapted)"
     */
    virtual std::string getType( ) const;

protected:
    //----------------------------------------------------------------------
    /**
     Scattering matrix of the R-type adapter

     Size:  (childNodeCount+1) x (childNodeCount+1)
            ("+1" for upfacing port)
     */
    std::unique_ptr<mat> S;

};

//==============================================================================
class wdfTerminatedSeries : public wdfTerminatedAdapter {

private:
    //----------------------------------------------------------------------
    /**
     Upfacing scattering coefficient of the series adapter
     */
    DspFloatType yu;
    /**
     Left scattering coefficient of the series adapter
     */
    DspFloatType yl;
    /**
     Right scattering coefficient of the series adapter
     */
    DspFloatType yr;

public:
    //----------------------------------------------------------------------
    /**
     Adapter class for terminated three port series nodes.

     The adapter owns one terminated upfacing port and two downfacing ports

     @param *left               holds a pointer to the left child of the node
     @param *right              holds a pointer to the right child of the node
     */
    wdfTerminatedSeries( wdfTreeNode *left,
                         wdfTreeNode *right );

    //----------------------------------------------------------------------
    /**
     Returns the series adaptors upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the series adaptation law Rup = Rleft + Rright.

     @param sampleRate          sample rate as needed to adapt
                                capacitors/inductors
     @returns                   a DspFloatType type port resistance of that element
                                in Ohms
     */
    virtual DspFloatType calculateUpRes( DspFloatType sampleRate );

    //----------------------------------------------------------------------
    /**
     Recursively calculates scattering coefficients of adapters.

     This recursion should be initiated from the base of the tree. It
     calculates the scattering coefficients according to the port resistances
     of the adaptor and the series adaptor scattering law.
     */
    virtual void calculateScatterCoeffs( );

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     It collects the wave components from the downfacing ports and returns the
     reflected wave.

     @returns                   the upward traveling wave of the node
     */
    virtual DspFloatType calculateUpB( );

    //----------------------------------------------------------------------
    /**
     Sets the wave components in the downfacing ports.

     This function weights the input wave and the incident waves on the
     downports with the coefficients yu, yl and yr and sets the reflected waves
     in the downfacing port objects.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( DspFloatType descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this adaptor.

     @returns                   a String describing the type of this adaptor as
                                "Series Adapter (TOP adapted)"
     */
    virtual std::string getType( ) const;

};

//==============================================================================
class wdfTerminatedParallel : public wdfTerminatedAdapter {

private:
    //----------------------------------------------------------------------
    /**
     Upfacing scattering coefficient of the parallel adapter
     */
    DspFloatType du;
    /**
     Left scattering coefficient of the parallel adapter
     */
    DspFloatType dl;
    /**
     Right scattering coefficient of the parallel adapter
     */
    DspFloatType dr;

public:
    //----------------------------------------------------------------------
    /**
     Adapter class for terminated three port parallel nodes.

     The adapter owns one adapted upfacing port and two downfacing ports

     @param *left               holds a pointer to the left child of the node
     @param *right              holds a pointer to the right child of the node
     */
    wdfTerminatedParallel( wdfTreeNode *left,
                           wdfTreeNode *right );

    //----------------------------------------------------------------------
    /**
     Returns the parallel adaptors upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the series adaptation law Gup = Gleft + Gright.

     @param sampleRate          sample rate as needed to adapt
                                capacitors/inductors
     @returns                   a DspFloatType type port resistance of that element
                                in Ohms
     */
    virtual DspFloatType calculateUpRes( DspFloatType sampleRate );

    //----------------------------------------------------------------------
    /**
     Recursively calculates scattering coefficients of adapters.

     This recursion should be initiated from the base of the tree. It
     calculates the scattering coefficients according to the port resistances
     of the adaptor and the parallel adaptor scattering law.
     */
    virtual void calculateScatterCoeffs( );

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     It collects the wave components from the downfacing ports and returns the
     reflected wave.

     @returns                   the upward traveling wave of the node
     */
    virtual DspFloatType calculateUpB();

    //----------------------------------------------------------------------
    /**
     Sets the wave components in the downfacing ports.

     This function weights the input wave and the incident waves on the
     downports with the coefficients du, dl and dr and sets the reflected waves
     in the downfacing port objects.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( DspFloatType descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this adaptor.

     @returns                   a String describing the type of this adaptor as
                                "Parallel Adapter (TOP adapted)"
     */
    virtual std::string getType( ) const;

};

class wdfInverter : public wdfTerminatedAdapter {

public:
    //----------------------------------------------------------------------
    /**
     Adapter class for terminated wave inverter nodes.

     The adapter owns one terminated upfacing port and one downfacing port

     @param *child              holds a pointer to the child of the inverter
     */
    wdfInverter( wdfTreeNode *child );

    //----------------------------------------------------------------------
    /**
     Returns the inverters upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the inverters adaptation law Rup = Rchild.

     @param sampleRate          sample rate as needed to adapt
                                capacitors/inductors
     @returns                   a DspFloatType type port resistance of that element
                                in Ohms
     */
    virtual DspFloatType calculateUpRes( DspFloatType sampleRate );

    //----------------------------------------------------------------------
    /**
     Recursively calculates scattering coefficients of adapters.

     This recursion should be initiated from the base of the tree. It
     does nothing for the inverter except continuing the recursion as the
     coefficients are hard-coded in the calculateUpB() and calculateDownB()
     functions.
     */
    virtual void calculateScatterCoeffs( );

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     It collects the wave from the downfacing port and returns it sign-flipped
     as the reflected wave.

     @returns                   the upward traveling wave of the node
     */
    virtual DspFloatType calculateUpB( );

    //----------------------------------------------------------------------
    /**
     Sets the wave component in the downfacing port.

     This function sets the sign-flipped incident wave on the downport.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( DspFloatType descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this adaptor.

     @returns                   a String describing the type of this adaptor as
                                "Inverter (TOP adapted)"
     */
    virtual std::string getType( ) const;

};

#pragma mark - Terminated Leafs
//==============================================================================
//                    T E R M I N A T E D   L E A F S   (Q)
//==============================================================================
class wdfTerminatedLeaf: public wdfTreeNode {

public:
    //----------------------------------------------------------------------
    /**
     Base class for adapted leafs in WDF subtrees/branches.

     This includes all linear one-port elements with one terminated upfacing
     port and non-implicit behaviour.
     */
    wdfTerminatedLeaf();
    
    //----------------------------------------------------------------------
    /**
     This variant of the base classes' calculateScatterCoeffs() function
     is an empty function to break the recursion that only applies
     to adapters and stops here at the leafs.
     */
    virtual void calculateScatterCoeffs( );

};

//==============================================================================
class wdfTerminatedCap : public wdfTerminatedLeaf {

public:
    //----------------------------------------------------------------------
    /**
     Adapted capacitor model class.

     Creates a capacitor with capacitance C.

     @param C                  physical capacitance of the component in Farads
     @param sampleRate         sample rate in Hertz
     */
    wdfTerminatedCap( DspFloatType C,
                      DspFloatType sampleRate );

    //----------------------------------------------------------------------
    /**
     Returns the capacitors upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the capacitors adaptation law Rup = 1 / ( 2.0 * sampleRate * C ).

     @param sampleRate          sample rate in Hertz as needed to
                                adapt the capacitor
     @returns                   a DspFloatType type port resistance of that element
                                in Ohms
     */
    virtual DspFloatType calculateUpRes( DspFloatType sampleRate );

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     It returns the incident wave delayed by one sample as the reflected wave.

     @returns                   the upward traveling wave of the node
     */
    virtual DspFloatType calculateUpB( );

    //----------------------------------------------------------------------
    /**
     Sets the wave component in the downfacing port.

     This function feeds a one sample delay with the incident wave to return
     it as the next reflected wave.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( DspFloatType descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this leaf.

     @returns                   a String describing the type of this leaf as
                                "C (adapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Capacitance in Farad
     */
    DspFloatType C;
    /**
     Sample rate in Hertz
     */
    DspFloatType sampleRate;
    /**
     One sample delay element
     */
    DspFloatType prevA;

};

//==============================================================================
class wdfTerminatedInd : public wdfTerminatedLeaf {

public:
    //----------------------------------------------------------------------
    /**
     Adapted inductor model class.

     Creates an inductor with inductance L.

     @param L                  physical inductance of the component in Henry
     @param sampleRate         sample rate in Hertz
     */
    wdfTerminatedInd( DspFloatType L,
                      DspFloatType sampleRate );

    //----------------------------------------------------------------------
    /**
     Returns the inductors upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the inverters adaptation law Rup = R = 2.0 * sampleRate L.

     @param sampleRate          sample rate in Hertz as needed to
                                adapt the capacitor
     @returns                   a DspFloatType type port resistance of that element
                                in Ohms
     */
    virtual DspFloatType calculateUpRes( DspFloatType sampleRate );

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     It returns the sign-flipped incident wave delayed by one sample as the
     reflected wave.

     @returns                   the upward traveling wave of the node
     */
    virtual DspFloatType calculateUpB( );

    //----------------------------------------------------------------------
    /**
     Sets the wave component in the downfacing port.

     This function feeds a one sample delay with the incident wave to return
     it sign-flipped as the next reflected wave.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( DspFloatType descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this leaf.

     @returns                   a String describing the type of this leaf as
                                "L (adapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Inductance in Henry
     */
    DspFloatType L;
    /**
     Sample rate in Hertz
     */
    DspFloatType sampleRate;
    /**
     One sample delay element
     */
    DspFloatType prevA;

};

//==============================================================================
class wdfTerminatedRes : public wdfTerminatedLeaf {

public:
    //----------------------------------------------------------------------
    /**
     Adapted resistor model class.

     Creates a resistor with resistance R.

     @param R                   physical resistance of the component in Ohm
     */
    wdfTerminatedRes( DspFloatType R );

    //----------------------------------------------------------------------
    /**
     Returns the resistors upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the resistors adaptation law Rup = R.

     @param sampleRate          sample rate in Hertz as needed to
                                adapt the capacitor
     @returns                   a DspFloatType type port resistance of that element
                                in Ohms
     */
    virtual DspFloatType calculateUpRes( DspFloatType sampleRate );

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     It always returns 0.0 for the adapted resistor.

     @returns                   the upward traveling wave of the node
     */
    virtual DspFloatType calculateUpB( );

    //----------------------------------------------------------------------
    /**
     Sets the wave component in the downfacing port.

     This function does nothing for the adapted resistor.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( DspFloatType descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this leaf.

     @returns                   a String describing the type of this leaf as
                                "R (adapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Resistance in Ohm
     */
    DspFloatType R;

};

//==============================================================================
class wdfTerminatedResVSource : public wdfTerminatedLeaf {

public:
    //----------------------------------------------------------------------
    /**
     Adapted resistive voltage source model class.

     Creates a voltage source with series resistance Rser.

     @param Vs                  initial source voltage in Volts
     @param Rser                physical resistance of the series resistor in
                                Ohms
     */
    wdfTerminatedResVSource( DspFloatType Vs,
                             DspFloatType RSer );

    //----------------------------------------------------------------------
    /**
     Returns the voltage source's upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the adaptation law Rup = Rser.

     @param sampleRate          sample rate in Hertz as needed to
                                adapt the capacitor
     @returns                   a DspFloatType type port resistance of that element
                                in Ohms
     */
    virtual DspFloatType calculateUpRes( DspFloatType sampleRate );

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     It returns the source voltage value as the reflected wave.

     @returns                   the upward traveling wave of the node
     */
    virtual DspFloatType calculateUpB( );

    //----------------------------------------------------------------------
    /**
     Sets the wave component in the downfacing port.

     This function does nothing for the adapted voltage source.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( DspFloatType descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this leaf.

     @returns                   a String describing the type of this leaf as
                                "Vs (incl. Rp = RSer -> adapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Source voltage in volts. Change this public variable during operation if
     necessary according to input samples.
     */
    DspFloatType Vs;
    /**
     Series resistance in Ohms.
     */
    DspFloatType RSer;

};

//==============================================================================
class wdfTerminatedResCSource : public wdfTerminatedLeaf
{
public:
    //----------------------------------------------------------------------
    /**
     Adapted resistive current source model class.

     Creates a current source with series resistance Rpar.

     @param Is                  initial source current in Amps
     @param Rpar                physical resistance of the parallel resistor in
                                Ohms
     */
    wdfTerminatedResCSource( DspFloatType Is,
                             DspFloatType RPar );

    //----------------------------------------------------------------------
    /**
     Returns the current source's upfacing port resistance.

     This function is called from adaptPorts(). It returns the upfacing
     port resistance of the node to fulfill termination according to
     the adaptation law Rup = Rpar.

     @param sampleRate          sample rate in Hertz as needed to
                                adapt the element
     @returns                   a DspFloatType type port resistance of that element
                                in Ohms
     */
    virtual DspFloatType calculateUpRes( DspFloatType sampleRate );

    //----------------------------------------------------------------------
    /**
     Returns the upfacing wave component towards the root.

     This function is called from pullWave() to calculate the wave that
     travels towards the base.

     @returns                   the upward traveling wave of the node
     */
    virtual DspFloatType calculateUpB( );

    //----------------------------------------------------------------------
    /**
     Sets the wave component in the downfacing port.

     This function does nothing for the adapted current source.

     @param  descendingWave     incoming wave component on the upfacing port
     */
    virtual void calculateDownB( DspFloatType descendingWave );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this leaf.

     @returns                   a String describing the type of this leaf as
                                "Is (incl. Rp = RPar -> adapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Source current in amps. Change this public variable during operation if
     necessary according to input samples.
     */
    DspFloatType Is;
    /**
     Parallel resistance in Ohms.
     */
    DspFloatType RPar;
};


#pragma mark - Root Nodes
//==============================================================================
//                             R O O T   N O D E
//==============================================================================
class wdfRootNode {

protected:
    //----------------------------------------------------------------------
    /**
     The WDF port resistance in Ohm.
     */
    DspFloatType Rp;
    /**
     The number of ports on a root node element.
     */
    size_t numPorts;

public:
    //----------------------------------------------------------------------
    /**
     Base class of a WDF Root Node.

     This is the base class for all unadapted WDF elements that can form the
     closed-form (non-implicit) root of a tree.

     @param numPorts            number of ports on this root node
     */
    wdfRootNode( int numPorts );

    //----------------------------------------------------------------------
    /**
     Deconstructor
     */
    virtual ~wdfRootNode( );

    //----------------------------------------------------------------------
    /**
     Sets the nodes port resistance according to the port it is connected
     to.

     As these nodes sit at the root, their port resistance is dependant on the
     upfacing port of the subtree they are connected to.

     @param Rp                  port resistance in Ohm
     */
    virtual void setPortResistance( DspFloatType Rp );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to calculate the reflection of
     the incoming, ascending wave.

     This is typically an unadapted expression which involves the component
     value of the node, the port resistance Rp, past states and the ascending
     wave itself.

     @param *ascendingWaves     Pointer to a vector of ascending waves
     @param *descendingWaves    Pointer to a vector to store descending waves
     @param *portIndex          Pointer to a counter with the port index to use
                                in the vectors of ascending and descending
                                waves. Needs to be incremented by numPorts in
                                the node's implementation.
     */
    virtual void calculateDownB( vec* ascendingWaves,
                                 vec* descendingWaves,
                                 size_t* portIndex ) = 0;

    //----------------------------------------------------------------------
    /**
     Return the number of ports of that root node.

     @returns                   the value of the numPorts member.
     */
    int getNumPorts( );

    //----------------------------------------------------------------------
    /**
     Virtual placeholder function that is meant to return a String describing
     the type of this root node.

     @returns                   a String describing the type of this root node
     */
    virtual std::string getType( ) const = 0;

};


//==============================================================================
//                  U N T E R M I N A T E D   E L E M E N T S
//==============================================================================
class wdfUnterminatedSwitch : public wdfRootNode {

private:
    //----------------------------------------------------------------------
    /**
     Position of the switch (open/closed)
    */
    int position;

public:
    //----------------------------------------------------------------------
    /**
     Unterminated switch (open/short) model class.

     @param position            can be either 1 for a closed switch or 0
                                for an open one.
     */
    wdfUnterminatedSwitch( int position );

    //----------------------------------------------------------------------
    /**
     Calculates the reflection of the incoming, ascending wave.

     This is either a complete reflection or it's inverse, depending on
     the switch position.

     @param *ascendingWaves     Pointer to a vector of ascending waves
     @param *descendingWaves    Pointer to a vector to store descending waves
     @param *portIndex          Pointer to a counter with the port index to use

                                in the vectors of ascending and descending
                                waves. Needs to be incremented by numPorts in
                                the node's implementation.
     */
    virtual void calculateDownB( vec* ascendingWaves,
                                 vec* descendingWaves,
                                 size_t* portIndex );

    //----------------------------------------------------------------------
    /**
     Sets the switch position (0/1)

     @param position            can be either 1 for a closed switch or 0
                                for an open one.
     */
    void setSwitch( int position );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root node.

     @returns                   a String describing the type of this leaf as
                                "Unadapted controllable switch"
     */
    virtual std::string getType( ) const;

};

#pragma mark - Unterminated Leafs
//==============================================================================
class wdfUnterminatedCap : public wdfRootNode {

protected:
    //----------------------------------------------------------------------
    /**
     Sample rate in Hertz
    */
    DspFloatType sampleRate;
    /**
     Place to store the previous incident wave component.
    */
    DspFloatType prevA;
    /**
     Place to store the previous reflected wave component.
    */
    DspFloatType prevB;
    /**
     Reflection coefficient, depending on the inherited port resistance,
     component value and sample rate.

     This coefficient is pre-calculated in the setPortResistance function.
    */
    DspFloatType reflectionCoeff;

public:
    //----------------------------------------------------------------------
    /**
     Unadapted capacitor model class.

     Creates an unadapted capacitor with capacitance C.

     @param C                  physical capacitance of the component in Farads
     @param sampleRate         sample rate in Hertz
    */
    wdfUnterminatedCap( DspFloatType C,
                        DspFloatType sampleRate );

    //----------------------------------------------------------------------
    /**

     Calculates the reflection of the incoming, ascending wave.

     This is composed of the recent ascending wave component and former
     components on this port.

     @param *ascendingWaves     Pointer to a vector of ascending waves
     @param *descendingWaves    Pointer to a vector to store descending waves
     @param *portIndex          Pointer to a counter with the port index to use
                                in the vectors of ascending and descending
                                waves. Needs to be incremented by numPorts in
                                the node's implementation.
    */
    virtual void calculateDownB( vec* ascendingWaves,
                                 vec* descendingWaves,
                                 size_t* portIndex );

    //----------------------------------------------------------------------
    /**

     Sets the nodes port resistance according to the port it is connected
     to.

     TODO: In the case of the unadapted capacitor, it also pre-calculates the
     reflection coefficient reflectionCoeff, which depends on the
     port resistance too.

     @param Rp                  port resistance in Ohm
     */
    virtual void setPortResistance( DspFloatType Rp );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root node.

     @returns                   a String describing the type of this leaf as
                                "C (unadapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Capacitance in Farad
    */
    DspFloatType C;

};

//==============================================================================
class wdfUnterminatedInd : public wdfRootNode {

protected:
    //----------------------------------------------------------------------
    /**
     Sample rate in Hertz
    */
    DspFloatType sampleRate;
    /**
     Place to store the previous incident wave component.
    */
    DspFloatType prevA;
    /**
     Place to store the previous reflected wave component.
    */
    DspFloatType prevB;
    /**
     Reflection coefficient, depending on the inherited port resistance,
     component value and sample rate.

     This coefficient is pre-calculated in the setPortResistance function.
    */
    DspFloatType reflectionCoeff;

public:
    //----------------------------------------------------------------------
    /**
     Unadapted inductor model class.

     Creates an unadapted inductor with capacitance L.

     @param L                  physical inductance of the component in Henry
     @param sampleRate         sample rate in Hertz
    */
    wdfUnterminatedInd( DspFloatType L,
                        DspFloatType sampleRate );

    //----------------------------------------------------------------------
    /**
     Calculates the reflection of the incoming, ascending wave.

     This is composed of the recent ascending wave component and former
     components on this port.

     @param *ascendingWaves     Pointer to a vector of ascending waves
     @param *descendingWaves    Pointer to a vector to store descending waves
     @param *portIndex          Pointer to a counter with the port index to use
                                in the vectors of ascending and descending
                                waves. Needs to be incremented by numPorts in
                                the node's implementation.
    */
    virtual void calculateDownB( vec* ascendingWaves,
                                 vec* descendingWaves,
                                 size_t* portIndex );

    //----------------------------------------------------------------------
    /**
     Sets the nodes port resistance according to the port it is connected
     to.

     TODO: In the case of the unadapted inductor, it also pre-calculates the
     reflection coefficient reflectionCoeff, which depends on the
     port resistance too.

     @param Rp                  port resistance in Ohm
     */
    virtual void setPortResistance( DspFloatType Rp );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root node.

     @returns                   a String describing the type of this leaf as
                                "L (unadapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Inductance in Henry
    */
    DspFloatType L;

};

//==============================================================================
class wdfUnterminatedRes: public wdfRootNode {

protected:
    //----------------------------------------------------------------------
    /**
     Reflection coefficient, depending on the inherited port resistance and
     component value.

     This coefficient is pre-calculated in the setPortResistance function.
    */
    DspFloatType reflectionCoeff;

public:
    //----------------------------------------------------------------------
    /**
     Unadapted resistor model class.

     Creates an unadapted resistor with resistance R.

     @param R                  physical inductance of the component in Henry
    */
    wdfUnterminatedRes( DspFloatType R );

    //----------------------------------------------------------------------
    /**
     Calculates the reflection of the incoming, ascending wave.

     @param *ascendingWaves     Pointer to a vector of ascending waves
     @param *descendingWaves    Pointer to a vector to store descending waves
     @param *portIndex          Pointer to a counter with the port index to use
                                in the vectors of ascending and descending
                                waves. Needs to be incremented by numPorts in
                                the node's implementation.
    */
    virtual void calculateDownB( vec* ascendingWaves,
                                 vec* descendingWaves,
                                 size_t* portIndex );

    //----------------------------------------------------------------------
    /**
     Sets the nodes port resistance according to the port it is connected
     to.

     TODO: In the case of the unadapted resistor, it also pre-calculates the
     reflection coefficient reflectionCoeff, which depends on the
     port resistance too.

     @param Rp                  port resistance in Ohm
     */
    virtual void setPortResistance( DspFloatType Rp );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root node.

     @returns                   a String describing the type of this leaf as
                                "R (unadapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Resistance in Ohms
    */
    DspFloatType R;

};

//==============================================================================
class wdfIdealVSource: public wdfRootNode {

public:
    //----------------------------------------------------------------------
    /**
     Unadapted, ideal voltage source class.

     Creates an ideal voltage source with initial voltage Vs

     @param Vs                  initial source voltage in Volts
    */
    wdfIdealVSource( DspFloatType Vs );

    //----------------------------------------------------------------------
    /**
     Calculates the reflection of the incoming, ascending wave.

     @param *ascendingWaves     Pointer to a vector of ascending waves
     @param *descendingWaves    Pointer to a vector to store descending waves
     @param *portIndex          Pointer to a counter with the port index to use
                                in the vectors of ascending and descending
                                waves. Needs to be incremented by numPorts in
                                the node's implementation.
    */
    virtual void calculateDownB( vec* ascendingWaves,
                                 vec* descendingWaves,
                                 size_t* portIndex );

    //----------------------------------------------------------------------
    /**
     Sets the nodes port resistance according to the port it is connected
     to.

     @param Rp                  port resistance in Ohm
     */
    virtual void setPortResistance( DspFloatType Rp );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root node.

     @returns                   a String describing the type of this leaf as
                                "Vs (ideal -> unadapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Source voltage in Volts
    */
    DspFloatType Vs;

};

//==============================================================================
class wdfIdealCSource: public wdfRootNode {

public:
    //----------------------------------------------------------------------
    /**
     Unadapted, ideal current source class.

     Creates an ideal current source with initial current Is

     @param Vs                  initial source voltage in Volts
    */
    wdfIdealCSource(DspFloatType I_in);

    //----------------------------------------------------------------------
    /**
     Calculates the reflection of the incoming, ascending wave.

     @param *ascendingWaves     Pointer to a vector of ascending waves
     @param *descendingWaves    Pointer to a vector to store descending waves
     @param *portIndex          Pointer to a counter with the port index to use
                                in the vectors of ascending and descending
                                waves. Needs to be incremented by numPorts in
                                the node's implementation.
    */
    virtual void calculateDownB( vec* ascendingWaves,
                                 vec* descendingWaves,
                                 size_t* portIndex );

    //----------------------------------------------------------------------
    /**
     Sets the nodes port resistance according to the port it is connected
     to.

     @param Rp                  port resistance in Ohm
     */
    virtual void setPortResistance( DspFloatType Rp );

    //----------------------------------------------------------------------
    /**
     Returns a String describing the type of this root node.

     @returns                   a String describing the type of this leaf as
                                "Is (ideal -> unadapted)"
     */
    virtual std::string getType( ) const;

    //----------------------------------------------------------------------
    /**
     Source current in Ampere
    */
    DspFloatType Is;

};





#pragma mark - Tree
//==============================================================================
//                                  T R E E
//==============================================================================
wdfTree::wdfTree( ) {
    root.reset();
    ascendingWaves.reset();
    descendingWaves.reset();
    treeSampleRate  = 1;
}

wdfTree::~wdfTree( ) {

}

//----------------------------------------------------------------------
void wdfTree::cycleWave( ) {
    for( unsigned int i = 0; i < subtreeCount; i++ ) {
        DspFloatType wave = subtreeEntryNodes[i]->pullWaveUp( );
        (*ascendingWaves)[i] = wave;
    }

    root->processAscendingWaves( ascendingWaves.get(), descendingWaves.get() );

    for( unsigned int i = 0; i < subtreeCount; i++ ) {
        subtreeEntryNodes[i]->pushWaveDown( (*descendingWaves)[i] );
    }
}

//----------------------------------------------------------------------
void wdfTree::initTree( ) {
    ascendingWaves.reset( new vec( subtreeCount ) );
    descendingWaves.reset( new vec( subtreeCount ) );

    for( unsigned int i = 0; i < subtreeCount; i++ ) {
        subtreeEntryNodes[i]->setParentInChildren( );
        subtreeEntryNodes[i]->createPorts( );
    }
}

//----------------------------------------------------------------------
void wdfTree::setSamplerate( DspFloatType fs ) {
    treeSampleRate  = fs;
}

//----------------------------------------------------------------------
DspFloatType wdfTree::getSamplerate( ) {
    return treeSampleRate;
}

//----------------------------------------------------------------------
int wdfTree::adaptTree( ) {
    for( unsigned int i = 0; i < subtreeCount; i++ ) {
        subtreeEntryNodes[i]->adaptPorts( treeSampleRate );
        Rp[i] = subtreeEntryNodes[i]->upPort->Rp;
        subtreeEntryNodes[i]->calculateScatterCoeffs( );
    }

    root->setPortResistances( Rp );

    matData* rootMatrixData = root->getRootMatrPtr( );
    if( rootMatrixData != NULL ){
        return setRootMatrData( rootMatrixData, Rp );
    }

    return 0;
}

//----------------------------------------------------------------------
const std::vector<paramData>& wdfTree::getParams( ) {
    return params;
}


#pragma mark - Roots
//==============================================================================
//                                 R O O T S
//==============================================================================
wdfRoot::wdfRoot( ) {

}

wdfRoot::~wdfRoot( ) {

}

//----------------------------------------------------------------------
void wdfRoot::setPortResistances( DspFloatType *Rp ) {
    //do nothing here, might be implemented by a subclass of wdfRoot..
}

matData* wdfRoot::getRootMatrPtr( ) {
    return NULL;
}

#pragma mark R-type Root
//==============================================================================
wdfRootRtype::wdfRootRtype( int numSubtrees ) : wdfRoot(),
                                                numSubtrees(numSubtrees) {
    rootMatrixData.reset( new matData );
    rootMatrixData->Smat.set_size( numSubtrees, numSubtrees );
    rootMatrixData->Emat.set_size(0, 0);
    rootMatrixData->Fmat.set_size(0, 0);
    rootMatrixData->Mmat.set_size(0, 0);
    rootMatrixData->Nmat.set_size(0, 0);
}

wdfRootRtype::~wdfRootRtype( ) {
    rootMatrixData.reset();
}

//----------------------------------------------------------------------
void wdfRootRtype::processAscendingWaves( vec* ascendingWaves,
                                          vec* descendingWaves ) {
    (*descendingWaves) = rootMatrixData->Smat * (*ascendingWaves);
}

//----------------------------------------------------------------------
matData* wdfRootRtype::getRootMatrPtr( ) {
    return rootMatrixData.get();
}

//----------------------------------------------------------------------
std::string wdfRootRtype::getType( ) const {
    return "Root (R-type)";
}


#pragma mark Non-Linear Root
//==============================================================================
wdfRootNL::wdfRootNL( int numSubtrees,
                      std::vector<int> nlList,
                      int solverType ) : wdfRoot( ),
                                         numSubtrees( numSubtrees ) {
    rootMatrixData.reset( new matData );

    // TODO make ENUM / MAP variant with different nlSolvers (!!)
    NlSolver.reset( new nlNewtonSolver( nlList, rootMatrixData.get() ) );
    int numNonlinearities = NlSolver->getNumPorts( );

    rootMatrixData->Smat.set_size( numSubtrees+numNonlinearities, numSubtrees+numNonlinearities );
    rootMatrixData->Emat.set_size( numNonlinearities, numSubtrees );
    rootMatrixData->Fmat.set_size( numNonlinearities, numNonlinearities );
    rootMatrixData->Mmat.set_size( numSubtrees, numSubtrees );
    rootMatrixData->Nmat.set_size( numSubtrees, numNonlinearities );
}

wdfRootNL::~wdfRootNL( ) {

}

//----------------------------------------------------------------------
void wdfRootNL::processAscendingWaves( vec* ascendingWaves,
                                       vec* descendingWaves ) {
    NlSolver->nlSolve( ascendingWaves, descendingWaves );
}

//----------------------------------------------------------------------
matData* wdfRootNL::getRootMatrPtr( ) {
    return rootMatrixData.get();
}

//----------------------------------------------------------------------
std::string wdfRootNL::getType( ) const {
    return "Root (NL-type)";
}


#pragma mark Simple Root
//==============================================================================
wdfRootSimple::wdfRootSimple( wdfRootNode* rootElement ): wdfRoot( ),
                                                          rootElement( rootElement ) {

}

//----------------------------------------------------------------------
void wdfRootSimple::setPortResistances( DspFloatType * Rp ) {
    rootElement->setPortResistance( Rp[0] );
}

//----------------------------------------------------------------------
void wdfRootSimple::processAscendingWaves( vec* ascendingWaves,
                                           vec* descendingWaves ) {
    size_t idx = 0;
    rootElement->calculateDownB( ascendingWaves, descendingWaves, &idx );
}

//----------------------------------------------------------------------
std::string wdfRootSimple::getType( ) const {
    return "Root (Simple-type)";
}


#pragma mark - Wave Port -
//==============================================================================
//                                  P O R T
//==============================================================================
wdfPort::wdfPort( wdfTreeNode *connectedNode ) : Rp( 0 ),
                                                 Gp( 0 ),
                                                 a( 0 ),
                                                 b( 0 ),
                                                 connectedNode( connectedNode ) {

}

//----------------------------------------------------------------------
DspFloatType wdfPort::getPortVoltage( ) {
    return ( a + b ) / 2.0;
}

//----------------------------------------------------------------------
DspFloatType wdfPort::getPortCurrent( ){
    return ( a - b ) / ( 2.0 * Rp );
}



#pragma mark - Tree Node -
//==============================================================================
//                             T R E E   N O D E
//==============================================================================

wdfTreeNode::wdfTreeNode( ) {
    upPort.reset( new wdfPort( NULL ) );
}

wdfTreeNode::wdfTreeNode( wdfTreeNode *left,
                          wdfTreeNode *right ) {
    childrenNodes.push_back( left );
    childrenNodes.push_back( right );
    upPort.reset( new wdfPort( NULL ) );
}

wdfTreeNode::wdfTreeNode( std::vector<wdfTreeNode*> childrenIn ) {
    for ( wdfTreeNode* child : childrenIn ) {
        childrenNodes.push_back( child );
    }
    upPort.reset( new wdfPort( NULL ) );
}

//----------------------------------------------------------------------
void wdfTreeNode::setParentInChildren( ) {
    for (wdfTreeNode* child : childrenNodes) {
        child->parentNode = this ;
        child->setParentInChildren( );
    }
}

//----------------------------------------------------------------------
void wdfTreeNode::createPorts( ) {
    for( unsigned int i = 0; i < childrenNodes.size(); i++) {
        downPorts.push_back( new wdfPort( childrenNodes[i] ) );
        childrenNodes[i]->upPort->connectedNode = this;
        childrenNodes[i]->createPorts( );
    }
}

//----------------------------------------------------------------------
DspFloatType wdfTreeNode::adaptPorts( DspFloatType sampleRate ) {
    for( wdfPort* downPort : downPorts ) {
        downPort->Rp = downPort->connectedNode->adaptPorts( sampleRate );
    }

    upPort->Rp = calculateUpRes( sampleRate );
    return upPort->Rp;
}

//----------------------------------------------------------------------
DspFloatType wdfTreeNode::pullWaveUp( ) {
    for( wdfPort* downPort : downPorts ) {
        downPort->a = downPort->connectedNode->pullWaveUp( );
    }

    upPort->b = calculateUpB( );
    return upPort->b;
}

//----------------------------------------------------------------------
void wdfTreeNode::pushWaveDown( DspFloatType descendingWave ) {
    upPort->a = descendingWave;
    calculateDownB( descendingWave );
    for( wdfPort* downPort : downPorts ) {
        downPort->connectedNode->pushWaveDown( downPort->b );
    }
}


#pragma mark - Terminated Adapters -
//==============================================================================
//              T E R M I N A T E D   A D A P T E R S   (S, P, R)
//==============================================================================
wdfTerminatedAdapter::wdfTerminatedAdapter( wdfTreeNode *left,
                                            wdfTreeNode *right ) : wdfTreeNode( left, right ) {
}

wdfTerminatedAdapter::wdfTerminatedAdapter(std::vector<wdfTreeNode*> childrenIn) : wdfTreeNode( childrenIn ) {

}


#pragma mark Terminated R-type Adapter
//==============================================================================
wdfTerminatedRtype::wdfTerminatedRtype( std::vector<wdfTreeNode*> childrenIn ) : wdfTerminatedAdapter( childrenIn ) {
    S.reset( new mat( childrenIn.size()+1, childrenIn.size()+1 ) );
}


wdfTerminatedRtype::~wdfTerminatedRtype( ) {

}

//  IMPLEMET THESE METHODS IN THE CIRCUIT TREE
// //----------------------------------------------------------------------
// DspFloatType wdfTerminatedRtype::calculateUpRes( DspFloatType sampleRate )
// {
//     const DspFloatType Rup = YOUR CODE HERE
//     return ( Rup );
// }
//
// //----------------------------------------------------------------------
// void wdfTerminatedRtype::calculateScatterCoeffs( )
// {
//     YOUR CODE HERE
//
//     for (int i = 0; i < childNodeCount; i++) {
//         downPorts[i]->connectedNode->calculateScatterCoeffs( );
//     }
// }

//----------------------------------------------------------------------
DspFloatType wdfTerminatedRtype::calculateUpB( )
{
    colvec inWaves( childrenNodes.size()+1 );
    inWaves(0) = 0;
    for( unsigned int i = 0; i < childrenNodes.size(); i++ ) {
        inWaves(i+1) = downPorts[i]->a;
    }
    DspFloatType upB = as_scalar( S->row(0) * inWaves );

    return upB;
}

//----------------------------------------------------------------------
void wdfTerminatedRtype::calculateDownB( DspFloatType descendingWave )
{
    colvec inWaves( childrenNodes.size()+1 );
    inWaves(0) = descendingWave;
    for( unsigned int i = 0; i < childrenNodes.size(); i++ ) {
        inWaves(i+1) = downPorts[i]->a;
    }

    colvec downWaves = (*S) * inWaves;

    for( unsigned int i = 0; i < childrenNodes.size(); i++ ) {
        downPorts[i]->b = downWaves(i+1);
    }
    int i=1;
    i++;
}

//----------------------------------------------------------------------
std::string wdfTerminatedRtype::getType( ) const
{
    return "R-type Adapter (TOP adapted)";
}


#pragma mark Terminated Series Adapter
//==============================================================================
wdfTerminatedSeries::wdfTerminatedSeries( wdfTreeNode *left,
                                          wdfTreeNode *right ) : wdfTerminatedAdapter(left, right),
                                                                 yu( 0 ),
                                                                 yl( 0 ),
                                                                 yr( 0 ) {

}

//----------------------------------------------------------------------
DspFloatType wdfTerminatedSeries::calculateUpRes( DspFloatType sampleRate ) {
    const DspFloatType Rleft  = downPorts[0]->Rp;
    const DspFloatType Rright = downPorts[1]->Rp;
    const DspFloatType Rser   = Rleft + Rright;
    return ( Rser );
}

//----------------------------------------------------------------------
void wdfTerminatedSeries::calculateScatterCoeffs( ) {
    const DspFloatType Ru = upPort->Rp;
    const DspFloatType Rl = downPorts[0]->Rp;
    const DspFloatType Rr = downPorts[1]->Rp;

    yu = 1.0;
    yl = 2.0 * Rl / ( Ru + Rl + Rr );
    yr = 1.0 - yl;

    for ( wdfPort* downPort : downPorts ) {
        downPort->connectedNode->calculateScatterCoeffs( );
    }
}

//----------------------------------------------------------------------
DspFloatType wdfTerminatedSeries::calculateUpB( ) {
    DspFloatType upB = -( downPorts[0]->a + downPorts[1]->a );
    return upB;
}

//----------------------------------------------------------------------
void wdfTerminatedSeries::calculateDownB( DspFloatType descendingWave ) {
    downPorts[0]->b = yl * ( downPorts[0]->a * ((1.0 / yl) - 1) - downPorts[1]->a - descendingWave );
    downPorts[1]->b = yr * ( downPorts[1]->a * ((1.0 / yr) - 1) - downPorts[0]->a - descendingWave );
}

//----------------------------------------------------------------------
std::string wdfTerminatedSeries::getType( ) const {
    return "Series Adapter (TOP adapted)";
}


#pragma mark Terminated Parallel Adapter
//==============================================================================
wdfTerminatedParallel::wdfTerminatedParallel( wdfTreeNode *left,
                                              wdfTreeNode *right ) : wdfTerminatedAdapter(left, right),
                                                                     du( 0 ),
                                                                     dl( 0 ),
                                                                     dr( 0 ) {

}

//----------------------------------------------------------------------
DspFloatType wdfTerminatedParallel::calculateUpRes( DspFloatType sampleRate ) {
    const DspFloatType Rleft  = downPorts[0]->Rp;
    const DspFloatType Rright = downPorts[1]->Rp;

    assert(Rleft > 0 && "Port resistance must be a nonzero positive number.");
    assert(Rright > 0 && "Port resistance must be a nonzero positive number.");
    
    const DspFloatType Rpar   = ( Rleft * Rright ) / ( Rleft + Rright );
    return Rpar;
}

//----------------------------------------------------------------------
void wdfTerminatedParallel::calculateScatterCoeffs( ) {
    const DspFloatType Gu = 1.0 / upPort->Rp;
    const DspFloatType Gl = 1.0 / downPorts[0]->Rp;
    const DspFloatType Gr = 1.0 / downPorts[1]->Rp;

    du = 1.0;
    dl = 2.0 * Gl / ( Gu + Gl + Gr );
    dr = 1.0 - dl;

    for ( wdfPort* downPort : downPorts ) {
        downPort->connectedNode->calculateScatterCoeffs( );
    }
}

//----------------------------------------------------------------------
DspFloatType wdfTerminatedParallel::calculateUpB( ) {
    return ( dl * downPorts[0]->a + dr * downPorts[1]->a );
}

//----------------------------------------------------------------------
void wdfTerminatedParallel::calculateDownB( DspFloatType descendingWave ) {
    downPorts[0]->b = ( ( dl - 1 ) * downPorts[0]->a + dr * downPorts[1]->a + du * descendingWave );
    downPorts[1]->b = ( dl * downPorts[0]->a + ( dr - 1 ) * downPorts[1]->a + du * descendingWave );
}

//----------------------------------------------------------------------
std::string wdfTerminatedParallel::getType( ) const {
    return "Parallel Adapter (TOP adapted)";
}

#pragma mark Inverter
//==============================================================================
wdfInverter::wdfInverter( wdfTreeNode *child ) :  wdfTerminatedAdapter( { child } ) {
}

//----------------------------------------------------------------------
DspFloatType wdfInverter::calculateUpRes( DspFloatType sampleRate ) {
    return downPorts[0]->Rp;
}

//----------------------------------------------------------------------
void wdfInverter::calculateScatterCoeffs( ) {
    for ( wdfPort* downPort : downPorts ) {
        downPort->connectedNode->calculateScatterCoeffs( );
    }
}

//----------------------------------------------------------------------
DspFloatType wdfInverter::calculateUpB( ) {
    return -1 * downPorts[0]->a ;
}

//----------------------------------------------------------------------
void wdfInverter::calculateDownB( DspFloatType descendingWave ) {
    downPorts[0]->b = -1 * descendingWave;
}

//----------------------------------------------------------------------
std::string wdfInverter::getType( ) const {
    return "Inverter (TOP adapted)";
}

#pragma mark - Terminated Leafs -
//==============================================================================
//                    T E R M I N A T E D   L E A F S   (Q)
//==============================================================================
wdfTerminatedLeaf::wdfTerminatedLeaf( ) : wdfTreeNode( ) {
}

//----------------------------------------------------------------------
void wdfTerminatedLeaf::calculateScatterCoeffs( ) {
    // this is the end of the recursive updateScatter() descent within the tree
};


#pragma mark Terminated Capacitor
//==============================================================================
wdfTerminatedCap::wdfTerminatedCap( DspFloatType C,
                                    DspFloatType sampleRate ) : wdfTerminatedLeaf( ),
                                                 C( C ),
                                                 sampleRate( sampleRate ),
                                                 prevA( 0 ) {

}

//----------------------------------------------------------------------
DspFloatType wdfTerminatedCap::calculateUpRes( DspFloatType sampleRate ) {
    assert(sampleRate > 0 && "sampleRate must be a nonzero positive number.");
    assert(C > 0 && "capacitance must be a nonzero positive number.");

    this->sampleRate = sampleRate;
    const DspFloatType R = 1 / ( 2.0 * sampleRate * C );
    return R;
}

//----------------------------------------------------------------------
DspFloatType wdfTerminatedCap::calculateUpB( ) {
    return prevA;
}

//----------------------------------------------------------------------
void wdfTerminatedCap::calculateDownB( DspFloatType descendingWave ) {
    prevA = descendingWave;
}

//----------------------------------------------------------------------
std::string wdfTerminatedCap::getType( ) const {
    return "C (adapted)";
}

#pragma mark Terminated Inductor
//==============================================================================
wdfTerminatedInd::wdfTerminatedInd( DspFloatType L,
                                    DspFloatType sampleRate ) : wdfTerminatedLeaf( ),
                                                 L( L ),
                                                 sampleRate( sampleRate ),
                                                 prevA( 0 ) {

}

//----------------------------------------------------------------------
DspFloatType wdfTerminatedInd::calculateUpRes( DspFloatType sampleRate ) {
    assert(sampleRate > 0 && "sampleRate must be a nonzero positive number.");
    assert(L > 0 && "inductance must be a nonzero positive number.");

    this->sampleRate = sampleRate;
    const DspFloatType R = 2.0 * sampleRate * L ;
    return R;
}

//----------------------------------------------------------------------
DspFloatType wdfTerminatedInd::calculateUpB( ) {
    return prevA;
}

//----------------------------------------------------------------------
void wdfTerminatedInd::calculateDownB( DspFloatType descendingWave ) {
    prevA = -1.0 * descendingWave;
}

//----------------------------------------------------------------------
std::string wdfTerminatedInd::getType( ) const {
    return "L (adapted)";
}

#pragma mark Terminated Resistor
//==============================================================================
wdfTerminatedRes::wdfTerminatedRes( DspFloatType R ) : wdfTerminatedLeaf( ),
                                                 R( R ) {

}

//----------------------------------------------------------------------
DspFloatType wdfTerminatedRes::calculateUpRes( DspFloatType sampleRate ) {
    return R;
}

//----------------------------------------------------------------------
DspFloatType wdfTerminatedRes::calculateUpB( ) {
    return 0.0;
}

//----------------------------------------------------------------------
void wdfTerminatedRes::calculateDownB( DspFloatType descendingWave ) {
    // do nothing, R is terminated/adapted!
}

//----------------------------------------------------------------------
std::string wdfTerminatedRes::getType( ) const {
    return "R (adapted)";
}

#pragma mark Terminated Resistive VSource
//==============================================================================
wdfTerminatedResVSource::wdfTerminatedResVSource( DspFloatType Vs,
                                                  DspFloatType RSer ) : wdfTerminatedLeaf( ),
                                                                  Vs( Vs ),
                                                                  RSer( RSer ) {

}

//----------------------------------------------------------------------
DspFloatType wdfTerminatedResVSource::calculateUpRes( DspFloatType sampleRate ) {
    return RSer;
}

//----------------------------------------------------------------------
DspFloatType wdfTerminatedResVSource::calculateUpB( ) {
    return Vs;
}

//----------------------------------------------------------------------
void wdfTerminatedResVSource::calculateDownB( DspFloatType descendingWave ) {
    // do nothing, VResVolt is terminated/adapted!
}

//----------------------------------------------------------------------
std::string wdfTerminatedResVSource::getType( ) const {
    return "Vs (incl. Rp = RSer -> adapted)";
}

#pragma mark Terminated Resistive CSource
//==============================================================================
wdfTerminatedResCSource::wdfTerminatedResCSource( DspFloatType Is,
                                                  DspFloatType RPar ) : wdfTerminatedLeaf( ),
                                                                  Is( Is ),
                                                                  RPar ( RPar ) {

}

//----------------------------------------------------------------------
DspFloatType wdfTerminatedResCSource::calculateUpRes( DspFloatType sampleRate ) {
    return RPar;
}

//----------------------------------------------------------------------
DspFloatType wdfTerminatedResCSource::calculateUpB( ) {
    return RPar * Is;
}

//----------------------------------------------------------------------
void wdfTerminatedResCSource::calculateDownB( DspFloatType descendingWave ) {
    // do nothing, node is terminated/adapted!
}

//----------------------------------------------------------------------
std::string wdfTerminatedResCSource::getType( ) const {
    return "Cs (incl. Rp = RPar -> adapted)";
}

#pragma mark - Unterminated Root Node -
//==============================================================================
//                             R O O T   N O D E
//==============================================================================
wdfRootNode::wdfRootNode( int numPorts ) {
    this->numPorts = numPorts;
}
//----------------------------------------------------------------------
wdfRootNode::~wdfRootNode( ) {

}

//----------------------------------------------------------------------
void wdfRootNode::setPortResistance( DspFloatType Rp ) {
    this->Rp = Rp;
}

int wdfRootNode::getNumPorts( ){
    return numPorts;
}

#pragma mark Unterminated Switch
//==============================================================================
//                  U N T E R M I N A T E D   E L E M E N T S
//==============================================================================
wdfUnterminatedSwitch::wdfUnterminatedSwitch( int position ) : wdfRootNode( 1 ),
                                                               position( position ) {

}

//----------------------------------------------------------------------
void wdfUnterminatedSwitch::calculateDownB( vec* ascendingWaves,
                                            vec* descendingWaves,
                                            size_t* portIndex ) {
    size_t idx   = (*portIndex);
    (*portIndex) = idx+numPorts;

    if( position == 0 ) // open
    {
        (*descendingWaves)[idx] = +1.0 * (*ascendingWaves)[idx];
    }
    else                // closed
    {
        (*descendingWaves)[idx] = -1.0 * (*ascendingWaves)[idx];
    }
}

//----------------------------------------------------------------------
void wdfUnterminatedSwitch::setSwitch( int position ) {
    this->position = position;
}

//----------------------------------------------------------------------
std::string wdfUnterminatedSwitch::getType( ) const {
    return "SW (unadapted)";
}

#pragma mark Unterminated Capacitor
//==============================================================================
wdfUnterminatedCap::wdfUnterminatedCap(DspFloatType C,
                                       DspFloatType sampleRate ) : wdfRootNode(1),
                                                    sampleRate(sampleRate),
                                                    prevA(0),
                                                    prevB(0),
                                                    C(C) {

}


//----------------------------------------------------------------------
void wdfUnterminatedCap::calculateDownB( vec* ascendingWaves,
                                         vec* descendingWaves,
                                         size_t* portIndex) {
    descendingWaves->at(*portIndex) = reflectionCoeff * prevB - reflectionCoeff * ascendingWaves->at(*portIndex) + prevA;
    prevB = descendingWaves->at(*portIndex);
    prevA = ascendingWaves->at(*portIndex);
    (*portIndex) += numPorts;
}

//----------------------------------------------------------------------
std::string wdfUnterminatedCap::getType( ) const {
    return "C (unadapted)";
}

void wdfUnterminatedCap::setPortResistance( DspFloatType Rp ) {
    this->Rp = Rp;
    reflectionCoeff = (Rp - 1 / (2 * sampleRate * C)) / (Rp + (1 / (2 * sampleRate * C)));
}

#pragma mark Unterminated Inductor
//==============================================================================
wdfUnterminatedInd::wdfUnterminatedInd( DspFloatType L,
                                        DspFloatType sampleRate ) : wdfRootNode(1),
                                                     sampleRate(sampleRate),
                                                     prevA(0),
                                                     prevB(0),
                                                     L(L) {

}


//----------------------------------------------------------------------
void wdfUnterminatedInd::calculateDownB( vec* ascendingWaves,
                                         vec* descendingWaves,
                                         size_t* portIndex) {
    descendingWaves->at(*portIndex) = -reflectionCoeff * prevB - reflectionCoeff * ascendingWaves->at(*portIndex) - prevA;
    prevB = descendingWaves->at(*portIndex);
    prevA = ascendingWaves->at(*portIndex);
    (*portIndex) += numPorts;
}

//----------------------------------------------------------------------
std::string wdfUnterminatedInd::getType( ) const {
    return "L (unadapted)";
}

void wdfUnterminatedInd::setPortResistance( DspFloatType Rp ) {
    this->Rp = Rp;
    reflectionCoeff = (Rp - 2 * sampleRate * L) / (Rp + 2 * sampleRate * L);
}


#pragma mark Unterminated Resistor
//==============================================================================
wdfUnterminatedRes::wdfUnterminatedRes( DspFloatType R ) : wdfRootNode(1),
                                                     R(R) {

}

//----------------------------------------------------------------------
void wdfUnterminatedRes::calculateDownB( vec* ascendingWaves,
                                         vec* descendingWaves,
                                         size_t* portIndex) {
    descendingWaves->at(*portIndex) = reflectionCoeff * ascendingWaves->at(*portIndex);
    (*portIndex) += numPorts;
}

//----------------------------------------------------------------------
std::string wdfUnterminatedRes::getType( ) const {
    return "R (unadapted)";
}

void wdfUnterminatedRes::setPortResistance( DspFloatType Rp ) {
    this->Rp = Rp;
    reflectionCoeff = (R - Rp) / (R + Rp);
}


#pragma mark Ideal Voltage Source
//==============================================================================
wdfIdealVSource::wdfIdealVSource( DspFloatType Vs ) : wdfRootNode(1),
                                                Vs(Vs) {

}

//----------------------------------------------------------------------
void wdfIdealVSource::calculateDownB( vec* ascendingWaves,
                                            vec* descendingWaves,
                                            size_t* portIndex) {
    descendingWaves->at(*portIndex) = 2 * Vs - ascendingWaves->at(*portIndex);
    (*portIndex) += numPorts;
}

//----------------------------------------------------------------------
std::string wdfIdealVSource::getType( ) const {
    return "Vs (ideal -> unadapted)";
}

//----------------------------------------------------------------------
void wdfIdealVSource::setPortResistance( DspFloatType Rp ) {
    this->Rp = Rp;
}

#pragma mark Ideal Current Source
//==============================================================================
wdfIdealCSource::wdfIdealCSource( DspFloatType Is ) : wdfRootNode(1),
                                                  Is(Is) {

}

//----------------------------------------------------------------------
void wdfIdealCSource::calculateDownB( vec* ascendingWaves,
                                      vec* descendingWaves,
                                      size_t* portIndex) {
    descendingWaves->at(*portIndex) = 2 * Rp * Is + ascendingWaves->at(*portIndex);
    (*portIndex) += numPorts;
}

//----------------------------------------------------------------------
std::string wdfIdealCSource::getType( ) const {
    return "Is (ideal -> unadapted)";
}

//----------------------------------------------------------------------
void wdfIdealCSource::setPortResistance( DspFloatType Rp ) {
    this->Rp = Rp;
}

//==============================================================================
// Parent class for nlModels
//==============================================================================
nlModel::nlModel( int numPorts ) : numPorts (numPorts) {

}

nlModel::~nlModel( ) {

}

//----------------------------------------------------------------------
int nlModel::getNumPorts( ) {
    return numPorts;
}


//==============================================================================
// Diode Models according to Kurt Werner et al
// ("An Improved and Generalized Diode Clipper Model for Wave Digital Filters")
//==============================================================================
#define Is_DIODE    2.52e-9
#define VT_DIODE    0.02585

diodeModel::diodeModel() : nlModel( 1 ) {

}

//----------------------------------------------------------------------
void diodeModel::calculate( vec* fNL,
                            mat* JNL,
                            vec* x,
                            int* currentPort ) {

    const DspFloatType vd = (*x)(*currentPort);
    const DspFloatType arg1 = vd/VT_DIODE;

    (*fNL)(*currentPort) = Is_DIODE*(exp(arg1)-1);
    (*JNL)(*currentPort,*currentPort) = (Is_DIODE/VT_DIODE)*exp(arg1);

    (*currentPort) = (*currentPort)+getNumPorts();
}

//==============================================================================
diodeApModel::diodeApModel( ) : nlModel( 1 ) {

}

//----------------------------------------------------------------------
void diodeApModel::calculate( vec* fNL,
                              mat* JNL,
                              vec* x,
                              int* currentPort) {

    const DspFloatType vd = (*x)(*currentPort);
    const DspFloatType arg1 = vd/VT_DIODE;

    (*fNL)(*currentPort) = Is_DIODE*(exp(arg1)-1)-Is_DIODE*(exp(-arg1)-1);
    (*JNL)(*currentPort,*currentPort) = (Is_DIODE/VT_DIODE)*(exp(arg1)+exp(-arg1));

    (*currentPort) = (*currentPort)+getNumPorts();
}


//==============================================================================
// Transistor Models using Ebers-Moll equations
// ("Large-signal behavior of junction transistors")
//==============================================================================
#define Is_BJT      5.911e-15
#define VT_BJT      0.02585
#define BETAF       1.434e3
#define BETAR       1.262
#define ALPHAF      (BETAF/(1.0+BETAF))     //TAKE CARE OF ( ) TO COMPILE CORRECTLY!!!!!! ARGHH!!
#define ALPHAR      (BETAR/(1.0+BETAR))     //TAKE CARE OF ( ) TO COMPILE CORRECTLY!!!!!!


npnEmModel::npnEmModel() : nlModel( 2 ) {

}

//----------------------------------------------------------------------
void npnEmModel::calculate( vec* fNL,
                            mat* JNL,
                            vec* x,
                            int* currentPort) {

    const DspFloatType vBC = (*x)(*currentPort);
    const DspFloatType vBE = (*x)((*currentPort)+1);

    const DspFloatType vBC_o_VT_BJT = vBC/VT_BJT;
    const DspFloatType vBE_o_VT_BJT = vBE/VT_BJT;
    const DspFloatType Is_BJT_o_VT_BJT = Is_BJT/VT_BJT;
    const DspFloatType Is_BJT_o_ALPHAR = Is_BJT/ALPHAR;
    const DspFloatType Is_BJT_o_ALPHAF = Is_BJT/ALPHAF;


    (*fNL)(*currentPort) = -Is_BJT*(exp(vBE_o_VT_BJT )-1)+(Is_BJT_o_ALPHAR)*(exp(vBC_o_VT_BJT)-1);
    (*JNL)((*currentPort),(*currentPort)) = (Is_BJT_o_ALPHAR/VT_BJT)*exp(vBC_o_VT_BJT);
    (*JNL)((*currentPort),((*currentPort)+1)) = (-Is_BJT_o_VT_BJT)*exp(vBE_o_VT_BJT );

    (*fNL)((*currentPort)+1) = (Is_BJT_o_ALPHAF)*(exp(vBE_o_VT_BJT )-1)-Is_BJT*(exp(vBC_o_VT_BJT)-1);
    (*JNL)(((*currentPort)+1),(*currentPort)) = (-Is_BJT_o_VT_BJT)*exp(vBC_o_VT_BJT);
    (*JNL)(((*currentPort)+1),((*currentPort)+1)) = (Is_BJT_o_ALPHAF/VT_BJT)*exp(vBE_o_VT_BJT );

    (*currentPort) = (*currentPort)+getNumPorts();
}


//==============================================================================
// Triode model according to Dempwolf et al
// ("A physically-motivated triode model for circuit simulations")
//==============================================================================
triDwModel::triDwModel() : nlModel( 2 ) {


}

//----------------------------------------------------------------------
void triDwModel::calculate( vec* fNL,
                            mat* JNL,
                            vec* x,
                            int* currentPort) {

    const DspFloatType G = 2.242E-3;
    const DspFloatType C = 3.40;
    const DspFloatType mu = 103.2;
    const DspFloatType y = 1.26;

    const DspFloatType Gg = 6.177E-4;
    const DspFloatType Cg = 9.901;
    const DspFloatType E = 1.314;
    const DspFloatType Ig0 = 8.025E-8;

    const DspFloatType vAC_mu = (*x)(*currentPort) / mu;
    const DspFloatType vGC = (*x)((*currentPort)+1);


    const DspFloatType exp_Cg_vGC = exp( Cg * vGC );
    const DspFloatType log_1_exp_Cg_vGC_Cg = (log( 1 + exp_Cg_vGC ) / Cg);


    // Ig
    (*fNL)((*currentPort)+1) = Gg * pow( log_1_exp_Cg_vGC_Cg, E ) + Ig0;

    // dIg / dvAC
    (*JNL)(((*currentPort)+1),(*currentPort)) = 0;
    // dIg / dvGC
    (*JNL)(((*currentPort)+1),((*currentPort)+1)) = ( Gg * E * exp_Cg_vGC *
                                                      pow( log_1_exp_Cg_vGC_Cg, (E-1)) ) /
                                                    (1 + exp_Cg_vGC);


    const DspFloatType exp_C_vAC_mu_vGC = exp( C * ( vAC_mu + vGC ));
    const DspFloatType pow_log_1_exp_C_vAC_mu_vGC_C_y_1 = pow( (log(1 + exp_C_vAC_mu_vGC) / C), (y-1));

    // Ik
    (*fNL)(*currentPort) = G * pow( log( 1 + exp_C_vAC_mu_vGC ) / C , y ) - (*fNL)((*currentPort)+1);

    // dIk / dvAC
    (*JNL)((*currentPort),(*currentPort)) = ( G * y * exp_C_vAC_mu_vGC *
                                              pow_log_1_exp_C_vAC_mu_vGC_C_y_1 ) /
                                            (mu * (1 + exp_C_vAC_mu_vGC));
    // dIk / dvGC
    (*JNL)((*currentPort),((*currentPort)+1)) = ( G * y * exp_C_vAC_mu_vGC *
                                                  pow_log_1_exp_C_vAC_mu_vGC_C_y_1 ) /
                                                (1 + exp_C_vAC_mu_vGC) - (*JNL)(((*currentPort)+1),((*currentPort)+1));


    (*currentPort) = (*currentPort)+getNumPorts();

}

//==============================================================================
// Parent class for nlSolvers
//==============================================================================
nlSolver::nlSolver( ) : numNLPorts( 0 ) {

}

nlSolver::~nlSolver( ) {

}

//----------------------------------------------------------------------
int nlSolver::getNumPorts( ) {
    return numNLPorts;
}


//==============================================================================
// Newton Solver
//==============================================================================
nlNewtonSolver::nlNewtonSolver( std::vector<int> nlList,
                        matData* myMatData ) : myMatData ( myMatData ) {

    // set up Vec<nlModel> nlModels properly according to std::vector<int> nlList
    for( int nlModel : nlList )
    {
        switch( nlModel ) {
            // Diodes:
            case DIODE:             // single diode
            {
                nlModels.push_back(new diodeModel);
                break;
            }
            case DIODE_AP:          // antiparallel diode pair
            {
                nlModels.push_back(new diodeApModel);
                break;
            }
            // Bipolar Transistors:
            case NPN_EM:            // Ebers-Moll npn BJT
            {
                nlModels.push_back(new npnEmModel);
                break;
            }
            // Triode Tubes:
            case TRI_DW:            // Dempwolf triode model
            {
                nlModels.push_back(new triDwModel);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    numNLPorts = 0;
    for ( nlModel* model : nlModels ) {
        numNLPorts += model->getNumPorts();
    }

    x0       = new vec(numNLPorts, fill::zeros);
    F        = new vec(numNLPorts, fill::zeros);
    J        = new mat(numNLPorts,numNLPorts, fill::zeros);
    fNL      = new vec(numNLPorts, fill::zeros);
    JNL      = new mat(numNLPorts,numNLPorts, fill::zeros);
    Fmat_fNL = new vec(numNLPorts, fill::zeros);

}

nlNewtonSolver::~nlNewtonSolver( ) {
    size_t modelCount = nlModels.size();
    for( size_t i = 0; i < modelCount; i++ ) {
        delete nlModels[i];
    }
    delete x0;
    delete F;
    delete J;
    delete fNL;
    delete JNL;
    delete Fmat_fNL;
}

//----------------------------------------------------------------------
void nlNewtonSolver::nlSolve( vec* inWaves,
                          vec* outWaves ) {

    DspFloatType iter = 0;            // # of iteration
    DspFloatType alpha = 0;

    (*J).zeros();

    if ( firstRun ) {
        firstRun = false;
    }
    else {
        (*x0) = (*Fmat_fNL) + (myMatData->Emat)*(*inWaves);
    }

    evalNlModels( inWaves, myMatData, x0 );


    DspFloatType normF = norm(*F);
    //printf("iter alpha         ||F||_2\n");
    //printf(" %3g %9.2e %14.7e\n", iter, alpha, normF);

    vec xnew;
    DspFloatType normFnew;

    while ( (normF >= TOL) && (iter < ITMAX) )
    {
        vec p = - (*J).i() * (*F);
        alpha = 1;
        xnew = (*x0) + alpha * p;
        evalNlModels(inWaves, myMatData, &xnew);
        normFnew = norm(*F);

        (*x0) = xnew;
        normF = normFnew;
        iter++;

    //        printf(" %3g %9.2e %14.7e\n", iter, alpha, normF);
    }

    (*outWaves) = (myMatData->Mmat) * (*inWaves) + (myMatData->Nmat) * (*fNL);

}

//----------------------------------------------------------------------
void nlNewtonSolver::evalNlModels( vec* inWaves,
                               matData* myMatData,
                               vec* x ) {
    int currentPort = 0;
    (*JNL).zeros();


    for ( nlModel* model : nlModels ) {
        model->calculate( fNL, JNL, x, &currentPort );
    }

    (*Fmat_fNL) = myMatData->Fmat*(*fNL);
    (*F) = (myMatData->Emat)*(*inWaves) + (*Fmat_fNL) - (*x);
    (*J) = (myMatData->Fmat)*(*JNL) - eye(size(*JNL));

}