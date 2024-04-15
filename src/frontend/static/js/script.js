// Define your buildings data
const buildings = [
    {id: 'ADMINISTRATIVE CENTER', name: 'Admin Center'},
    {id: 'AFRICAN AMERICAN HISTORY AND CULTURE HOUSE', name: 'African American Hist and Culture House'},
    {id: 'ATTERBURY STUDENT SUCCESS CENTER', name: 'Atterbury Success Center'},
    {id: 'FAMILY DEVELOPMENT CENTER', name: 'Family Development Center'},
    {id: 'BLOCH (HENRY W. ) MANAGEMENT BUILDING', name: 'Block Executive Hall'},
    {id: 'ENTREPRENEURSHIP AND INNOVATION', name: 'Bloch Heritage Hall'},
    {id: 'BROOKSIDE 51/STUDENT SERVICES SUITE', name: 'Brookeside 51'},
    {id: 'CHERRY HALL', name: 'Cherry Hall'},
    {id: 'COCKEFAIR HALL', name: 'Cockefair Hall'},
    {id: 'DEBATE HOUSE', name: 'Debate House'},
    {id: 'AND PSYCHOLOGICAL SCIENCES', name: 'Psychology Building'},
    {id: 'FINE ARTS BUILDING', name: 'Fine Arts Building'},
    {id: 'FLARSHEIM HALL', name: 'Flarsheim Hall'},
    {id: 'GENERAL SERVICES BUILDING', name: 'General Services Building'},
    {id: 'GRANT HALL', name: 'Grant Hall'},
    {id: 'HAAG HALL', name: 'Haag Hall'},
    {id: 'JOHNSON (HERMAN AND DOROTHY) RESIDENCE HALL', name: 'Johnson Residence Hall'},
    {id: 'KATZ HALL', name: 'Katz Hall'},
    {id: 'LAW BUILDING', name: 'Law Building'},
    {id: 'MANHEIM HALL', name: 'Manheim Hall'},
    {id: 'MILLER NICHOLS LIBRARY AND LEARNING CENTER', name: 'Miller Nichols Library'},
    {id: 'NEWCOMB HALL', name: 'Newcomb Hall'},
    {id: 'OAK STREET RESIDENCE HALL', name: 'Oak Residence Hall'},
    {id: 'OLSON (JAMES C.) PERFORMING ARTS CENTER', name: 'Olsen Performing Arts Center'},
    {id: 'PLASTER (ROBERT W.) FREE ENTERPRISE AND RESEARCH CENTER', name: 'Plaster Free Enterprise'},
    {id: 'ROYALL HALL', name: 'Royall Hall'},
    {id: 'SPENCER HALL', name: 'Spencer Hall'},
    {id: 'SCOFIELD HALL', name: 'Scofield Hall'},
    {id: 'STUDENT UNION', name: 'Student Union'},
    {id: 'SWINNEY CENTER', name: 'Swinney Center'},
    {id: 'NATIONAL MUSEUM OF TOY AND MINIATURES', name: 'Toys and Miniatures Museum'},
    {id: 'UNIVERSITY HOUSE', name: 'University House'},
];

function adjustMarkers() {
    const container = document.getElementById('map-container');
    const containerWidth = container.offsetWidth;
    const containerHeight = container.offsetHeight;
  
    document.querySelectorAll('.clickable-area').forEach(area => {
      const xPercent = parseFloat(getComputedStyle(area).getPropertyValue('--x-percent'));
      const yPercent = parseFloat(getComputedStyle(area).getPropertyValue('--y-percent'));
      area.style.left = (containerWidth * (xPercent / 100)) + 'px';
      area.style.top = (containerHeight * (yPercent / 100)) + 'px';
    });
  }

window.addEventListener('resize', function() {
    if (document.getElementById('map-container').offsetWidth / document.getElementById('map-container').offsetHeight !== aspectRatio) {
        adjustMarkers();
    }
});

let aspectRatio = document.getElementById('map-container').offsetWidth / document.getElementById('map-container').offsetHeight;

function adjustMarkersUp() {
    const adjustmentValue = 8; // This value represents the percentage to move up

    document.querySelectorAll('.clickable-area').forEach(area => {
        let currentTop = parseFloat(getComputedStyle(area).getPropertyValue('--y-percent'));
        area.style.setProperty('--y-percent', `${currentTop - adjustmentValue}%`);
    });
}

document.getElementById('submitPath').addEventListener('click', adjustMarkersUp);

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
    const apiUrl = 'http://localhost:8080/'; // Change to your actual API endpoint
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
        displayPathDetails(data.path, data.distance); // Update UI with the path and distance
    })
    .catch(error => {
        console.error('Error making API call:', error);
    });
}

function displayPathDetails(path, distance) {
    // Assuming path is an array of strings and distance is a number
    const pathElement = document.getElementById('routePath');
    const distanceElement = document.getElementById('routeDistance');
    const detailsElement = document.getElementById('pathDetails');

    pathElement.innerHTML = 'Path: ' + path.join(' → ');
    distanceElement.innerHTML = 'Total Distance: ' + distance + ' meters';

    detailsElement.style.display = 'block'; // Show the route details
}

/* async function fetchPoints() {
    const response = await fetch('../../../src/graph.txt');
    const data = await response.text();
    const points = data.split('\n').map(line => {
        const [label, x, y] = line.split(',');
        return { label, x: parseInt(x, 10), y: parseInt(y, 10) };
    });
    return points;
}

fetchPoints().then(points => {
    const mapContainer = document.getElementById('map-container');
    points.forEach(point => {
        const pointDiv = document.createElement('div');
        pointDiv.style.position = 'absolute';
        pointDiv.style.left = `${point.x}px`;
        pointDiv.style.top = `${point.y}px`;
        pointDiv.style.width = '10px';
        pointDiv.style.height = '10px';
        pointDiv.style.background = 'red';
        pointDiv.style.borderRadius = '50%';
        pointDiv.title = point.label;

        mapContainer.appendChild(pointDiv);
    });
}); */