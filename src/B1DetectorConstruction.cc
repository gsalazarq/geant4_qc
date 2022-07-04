//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
/// \file B1DetectorConstruction.cc
/// \brief Implementation of the B1DetectorConstruction class

#include "B1DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"

// for geometry definitions
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"

// for having units and constants
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"


// for material definitions
#include "G4Material.hh"
#include "G4NistManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::B1DetectorConstruction()
  : G4VUserDetectorConstruction(),
  fScoringVolume(0)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::~B1DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B1DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;




  // I. CREATE MATERIAL

  //chip material
  G4Material* mat_chip1 = nist->FindOrBuildMaterial("G4_Si");

  // Material for the world: low density hydrogen defined by "hand"
  G4double zet      = 1.0;
  G4double amass    = 1.01*g/mole;
  G4double density  = universe_mean_density;
  G4double pressure = 3.e-18*pascal;
  G4double tempture = 2.73*kelvin;
  G4Material* materialWorld
         = new G4Material("Galactic", zet, amass, density,
                          kStateGas, tempture, pressure);


  // II. CREATE GEOMETRY:

  // Envelope parameters
  //
  G4double env_sizeXY = 6*cm, env_sizeZ =  6*cm;
  //G4Material* env_mat = nist->FindOrBuildMaterial("G4_AIR");


  //
  // World
  //
  G4double world_sizeXY = 1.0*env_sizeXY;
  G4double world_sizeZ  = 1.0*env_sizeZ;
  //G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

  G4Box* solidWorld =
    new G4Box("World",                                     //its name
       1*world_sizeXY, 1*world_sizeXY, 1*world_sizeZ);     //its size

  G4LogicalVolume* logicWorld =
    new G4LogicalVolume(solidWorld,          //its solid
                        materialWorld,           //its material
                        "World");            //its name

  G4VPhysicalVolume* physWorld =
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

//
// Envelope
//
  G4Box* solidEnv =
    new G4Box("Envelope",                    //its name
              0.9*env_sizeXY, 0.9*env_sizeXY, 0.9*env_sizeZ); //its size

  G4LogicalVolume* logicEnv =
    new G4LogicalVolume(solidEnv,            //its solid
                        materialWorld,             //its material
                        "Envelope");         //its name

  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(),         //at (0,0,0)
                    logicEnv,                //its logical volume
                    "Envelope",              //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking

  //
  // Shape 1 : shielding 

  // Shape 2: semiconductor waffer
  //

  // II. CREATE GEOMETRY:
  G4double targetXSize  = 1.0*cm;
  G4double targetYZSize = 1.0*targetXSize;
  G4Box* targetSolid
         = new G4Box("solid-Target",    // name
                   0.25*targetXSize,   // half x-size
                   0.1*targetYZSize,  // half y-size
                   0.25*targetYZSize); // half z-size
  G4LogicalVolume* targetLogical
         = new G4LogicalVolume(targetSolid,     // solid
                               mat_chip1,  // material
                               "logic-Target"); // name
  new G4PVPlacement         (0,                 // (no) rotation
                             G4ThreeVector(), // translation
                             targetLogical,           // its logical volume
                             "Target",                // its name
                             logicWorld,            // its mother volume
                             false,                   // not used
                             0,                       // copy number
                             checkOverlaps);      // enable overlap check

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  // Set Shape2 as scoring volume
  //
  fScoringVolume = targetLogical;

  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
