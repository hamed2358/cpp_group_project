# Audio Controller – Components

A – CameraInput → Hämtar kamerabilden  
B – MarkerDetector → Hittar markern  
C – MarkerRecognizer → Känner igen markern  
D – PoseEstimator → Beräknar position och rotation  
E – AudioController → Styr ljud, pitch och volym
## Component Responsibilities
### A – CameraInput

**Beskrivning:**  

CameraInput är den första delen av programmet och ansvarar för kontakten med kameran. Komponenten ska öppna kameran, läsa in nya kamerabilder (frames) och kontrollera att bilderna är giltiga. När en fungerande bild har hämtats skickas den vidare till nästa komponent för fortsatt behandling. Om kameran inte kan öppnas eller en bild inte kan läsas ska komponenten kunna upptäcka detta och rapportera problemet.

**Samarbete:**  

CameraInput samarbetar främst med MarkerDetector. CameraInput levererar kamerabilden och MarkerDetector använder bilden för att börja leta efter markern. Personen som ansvarar för CameraInput behöver därför komma överens med MarkerDetector-ansvarig om hur bilden ska lämnas vidare mellan komponenterna.

**Ska inte göra:**  

CameraInput ska inte leta efter fyrkanter, känna igen markern, beräkna markerens position/rotation eller styra ljudet. Dess ansvar slutar när en fungerande kamerabild har lämnats vidare.

**Filer:**  

CameraInput.hpp – komponentens interface, alltså vad andra delar av programmet kan använda.  

CameraInput.cpp – själva implementationen som öppnar kameran och hämtar bilder.

**Header i CameraInput.cpp:**

```cpp

/*

Component: CameraInput

Primary owner: [Namn]

Checkpoint: [Checkpoint]

Responsibility: Capture valid camera frames for the processing pipeline.

Main contributions: Camera setup, frame capture and input validation.

*/
```

**Kort flöde:**  

Kamera → CameraInput → giltig frame → MarkerDetector


### B – MarkerDetector

**Beskrivning:**  

MarkerDetector ansvarar för att ta emot kamerabilden från CameraInput och leta efter möjliga markers i bilden. Komponenten bearbetar bilden, hittar områden och konturer och letar efter former som kan vara en fyrkantig marker. När en lämplig fyrkant hittas ska komponenten hitta dess fyra hörn och skicka informationen vidare. Den säger alltså ännu inte säkert att fyrkanten är vår marker, utan hittar möjliga marker-kandidater.

**Samarbete:**  

MarkerDetector tar emot kamerabilden från CameraInput och skickar den hittade fyrkanten och dess hörn vidare till MarkerRecognizer. Personen som ansvarar för MarkerDetector behöver därför samarbeta med både CameraInput- och MarkerRecognizer-ansvariga om vilken information som tas emot och skickas vidare.

**Ska inte göra:**  

MarkerDetector ska inte bestämma markerens identitet, beräkna dess pose eller styra ljudet. Dess ansvar är främst att hitta en lämplig fyrkant och dess hörn.

**Filer:**  

MarkerDetector.hpp – komponentens interface, alltså vad andra delar av programmet kan använda.  

MarkerDetector.cpp – själva implementationen för bildbearbetning, konturer, fyrkantskontroll och hörn.

**Header i MarkerDetector.cpp:**

```cpp
/*
Component: MarkerDetector
Primary owner: [Namn]
Checkpoint: [Checkpoint]
Responsibility: Find marker candidates and their corners in the camera image.
Main contributions: Image processing, contour detection, quadrilateral validation and corner detection.
*/
```

**Kort flöde:**  

CameraInput → kamerabild → MarkerDetector → fyrkant + 4 hörn → MarkerRecognizer

### C – MarkerRecognizer

**Beskrivning:**  

MarkerRecognizer ansvarar för att ta emot marker-kandidaten från MarkerDetector och kontrollera om den verkligen är vår marker. Komponenten gör den hittade fyrkanten till en normaliserad, rak bild och jämför mönstret med det kända marker-mönstret. Om mönstret stämmer ska komponenten identifiera markern och bestämma dess orientation, alltså hur den är roterad.

**Samarbete:**  

MarkerRecognizer tar emot fyrkanten och dess hörn från MarkerDetector. När markern har blivit godkänd skickas information om den vidare till PoseEstimator, som använder hörnen, markerinformationen och kamerainformationen för att beräkna markerens pose.

**Ska inte göra:**  

MarkerRecognizer ska inte hämta kamerabilder, leta efter fyrkanter från början, beräkna den slutliga positionen/pose eller styra ljudet. Dess huvudansvar är att avgöra om kandidaten är rätt marker och bestämma dess identitet och orientation.

**Filer:**  

MarkerRecognizer.hpp – komponentens interface, alltså vad andra delar av programmet kan använda.  

MarkerRecognizer.cpp – själva implementationen för normalisering, jämförelse med känt marker-mönster och recognition.

**Header i MarkerRecognizer.cpp:**

```cpp
/*
Component: MarkerRecognizer
Primary owner: [Namn]
Checkpoint: [Checkpoint]
Responsibility: Recognise the marker and determine its identity and orientation.
Main contributions: Marker normalisation, pattern matching and orientation detection.
*/
```

**Kort flöde:**  

MarkerDetector → fyrkant + hörn → MarkerRecognizer → godkänd marker + orientation → PoseEstimator


### D – PoseEstimator

**Beskrivning:**  

PoseEstimator ansvarar för att beräkna var markern befinner sig och hur den är riktad i förhållande till kameran. Komponenten använder information om den godkända markern, dess hörn och kamerans kalibreringsinformation. Resultatet blir användbara värden för markerens position och rotation, alltså dess pose. Dessa värden behövs för att AudioController ska kunna koppla markerens rörelse till ljudet.

**Samarbete:**  

PoseEstimator tar emot den godkända markerinformationen från MarkerRecognizer. Den skickar sedan färdiga värden, till exempel position/avstånd och rotation, vidare till AudioController.

**Ska inte göra:**  

PoseEstimator ska inte öppna kameran, hitta fyrkanter, avgöra om kandidaten är rätt marker eller spela/styra ljudet. Den ska framför allt omvandla informationen om den redan identifierade markern till användbar position och rotation.

**Filer:**  

PoseEstimator.hpp – komponentens interface, alltså vad andra delar av programmet kan använda.  

PoseEstimator.cpp – själva implementationen för homography, kameratransformation och beräkning/förfining av pose.

**Header i PoseEstimator.cpp:**

```cpp
/*
Component: PoseEstimator
Primary owner: [Namn]
Checkpoint: [Checkpoint]
Responsibility: Estimate the marker position and rotation relative to the camera.
Main contributions: Homography, camera transformation and pose estimation.
*/
```

**Kort flöde:**  

MarkerRecognizer → godkänd marker + hörn → PoseEstimator → position/avstånd + rotation → AudioController


### E – AudioController

**Beskrivning:**  

AudioController ansvarar för själva ljudet och interaktionen i projektet. Komponenten tar emot information om markern och använder den för att styra ljudet enligt vår Mini Proposal. När markern syns ska ljudet kunna spelas och när markern försvinner ska ljudet stoppas. Markerens avstånd från kameran ska användas för att ändra pitch, och rotationen ska kunna användas för att ändra volymen.

**Samarbete:**  

AudioController tar främst emot färdig information från PoseEstimator, till exempel markerens position/avstånd och rotation. Den behöver också veta om markern är synlig eller har försvunnit. Personen som ansvarar för AudioController behöver därför samarbeta med PoseEstimator-ansvarig om vilka värden som ska skickas till ljuddelen.

**Ska inte göra:**  

AudioController ska inte öppna kameran, leta efter fyrkanter, känna igen markern eller själv beräkna markerens pose. Den ska använda den färdiga markerinformationen för att styra ljudet.

**Filer:**  

AudioController.hpp – komponentens interface, alltså vad andra delar av programmet kan använda.  

AudioController.cpp – själva implementationen för att spela/stoppa ljud och ändra pitch och volym.

**Header i AudioController.cpp:**

```cpp
/*
Component: AudioController
Primary owner: [Namn]
Checkpoint: [Checkpoint]
Responsibility: Control audio using information from the recognised marker.
Main contributions: Play/stop control, pitch control and volume control.
*/
```

**Kort flöde:**  

PoseEstimator → marker synlig + avstånd + rotation → AudioController → play/stop + pitch + volume