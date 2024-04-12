// Define your buildings data
const buildings = [
    {id: 'admincenter', name: 'Admin Center'},
    {id: 'africanamericanhistandculturehouse', name: 'African American Hist and Culture House'},
    {id: 'atterburysuccesscenter', name: 'Atterbury Success Center'},
    {id: 'familydevelopmentcenter', name: 'Family Development Center'},
    {id: 'blochexecutivehall', name: 'Block Executive Hall'},
    {id: 'blochheritagehall', name: 'Bloch Heritage Hall'},
    {id: 'brookeside51', name: 'Brookeside 51'},
    {id: 'cherryhall', name: 'Cherry Hall'},
    {id: 'cockefairhall', name: 'Cockefair Hall'},
    {id: 'debatehouse', name: 'Debate House'},
    {id: 'psychologybuilding', name: 'Psychology Building'},
    {id: 'fineartsbuilding', name: 'Fine Arts Building'},
    {id: 'flarsheimhall', name: 'Flarsheim Hall'},
    {id: 'generalservicesbldg', name: 'General Services Building'},
    {id: 'granthall', name: 'Grant Hall'},
    {id: 'haaghall', name: 'Haag Hall'},
    {id: 'johnsonresidencehall', name: 'Johnson Residence Hall'},
    {id: 'katzhall', name: 'Katz Hall'},
    {id: 'lawbldg', name: 'Law Building'},
    {id: 'manheimhall', name: 'Manheim Hall'},
    {id: 'millernicholslibrary', name: 'Miller Nichols Library'},
    {id: 'newcombhall', name: 'Newcomb Hall'},
    {id: 'oakresidencehall', name: 'Oak Residence Hall'},
    {id: 'olsenperformingartscenter', name: 'Olsen Performing Arts Center'},
    {id: 'plasterfreeenterprise', name: 'Plaster Free Enterprise'},
    {id: 'royallhall', name: 'Royall Hall'},
    {id: 'spencerhall', name: 'Spencer Hall'},
    {id: 'scofieldhall', name: 'Scofield Hall'},
    {id: 'studentunion', name: 'Student Union'},
    {id: 'swinneycenter', name: 'Swinney Center'},
    {id: 'toysandminiaturesmuseum', name: 'Toys and Miniatures Museum'},
    {id: 'universityhouse', name: 'University House'},
];

// Function to initialize click listeners
function initMapClickListeners() {
    document.getElementById('map-container').addEventListener('click', function(event) {
        handleMapClick(event, this);
    });
}

function handleMapClick(event, mapContainer) {
    var mapWidth = mapContainer.offsetWidth;
    var mapHeight = mapContainer.offsetHeight;

    var xPercent = (event.offsetX / mapWidth) * 100;
    var yPercent = (event.offsetY / mapHeight) * 100;

    // Formatting the output string as specified
    var formattedOutput = `--y-percent: ${yPercent.toFixed(2)}%;\n--x-percent: ${xPercent.toFixed(2)}%;`;

    console.log(formattedOutput);
}


// Function to find which building was clicked based on x and y percentages
function findClickedBuilding(xPercent, yPercent) {
    return buildings.find(building => 
        xPercent >= building.left && 
        xPercent <= (building.left + building.width) &&
        yPercent >= building.top && 
        yPercent <= (building.top + building.height));
}

let startLocation = null;
let destinationLocation = null;

function attachClickListenersToAreas() {
    document.querySelectorAll('.clickable-area').forEach(function(area) {
        area.addEventListener('click', function() {
            const buildingId = this.id; // 'this' correctly refers to 'area' here due to function() usage
            
            if (buildingId === startLocation || buildingId === destinationLocation) {
                if (buildingId === startLocation) {
                    startLocation = null;
                    this.classList.remove('start-location');
                } else if (buildingId === destinationLocation) {
                    destinationLocation = null;
                    this.classList.remove('destination-location');
                }
                console.log('Location unselected.');
            } else {
                if (!startLocation) {
                    startLocation = buildingId;
                    this.classList.add('start-location');
                    console.log(`Start location set to: ${buildingId}`);
                } else if (!destinationLocation) {
                    destinationLocation = buildingId;
                    this.classList.add('destination-location');
                    console.log(`Destination set to: ${buildingId}`);
                    // Optional: trigger route calculation or display logic here if both start and destination are selected
                }
            }
        });
    });
}

document.addEventListener('DOMContentLoaded', () => {
    initMapClickListeners();
    attachClickListenersToAreas();
    setupSubmitButton();
});

function setupSubmitButton() {
    document.getElementById('submitPath').addEventListener('click', function() {
        if (startLocation && destinationLocation) {
            submitLocationsToAPI(startLocation, destinationLocation);
        } else {
            console.log('Please select both a start and a destination location.');
        }
    });
}

function submitLocationsToAPI(start, end) {
    const apiUrl = 'http://localhost:8080/api/find-path'; // Change to your actual API endpoint
    fetch(apiUrl, {
        method: 'POST',
        mode: 'cors',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ start: start, end: end }),
    })
    .then(response => {
        if (!response.ok) {
            throw new Error('Network response was not ok ' + response.statusText);
        }
        return response.json();  // convert response to JSON
    })
    .then(data => {
        console.log('API Response:', data);  // log data to the console
        // Further processing here
    })
    .catch(error => {
        console.error('Error making API call:', error);
    });
}


