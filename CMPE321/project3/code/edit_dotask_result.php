<html>
<head>
</head>
<body>

    <?php
        $servername = "localhost";
        $username = "root";
        $password = "";
        $dbname = "cmpe321hw3";

        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);

        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        }else{

            $constraintSql = "SELECT * FROM task as newtask WHERE newtask.ID = " . $_POST['NewTaskID'] . " AND EXISTS (SELECT * FROM task WHERE task.ID IN ( SELECT dotask.TaskID as ID FROM dotask WHERE NOT dotask.TaskID = " . $_POST['TaskID'] . " AND dotask.EmployeeID = " . $_POST['NewEmployeeID'] . " ) AND task.StartDate <= ADDDATE(newtask.StartDate,INTERVAL newtask.DayItTakes-1 DAY) AND ADDDATE(task.StartDate,INTERVAL task.DayItTakes-1 DAY) >= newtask.StartDate )";

            $result = $conn->query($constraintSql);
            if($result->num_rows > 0){
                echo "An employee cannot be assigned to multiple tasks for the same day.";
            }else{
                // Update the record
                $sql = "UPDATE dotask SET TaskID = '" . $_POST['NewTaskID'] . "', EmployeeID = '" . $_POST['NewEmployeeID'] .
                    "' WHERE TaskID = " . $_POST['TaskID'] . " AND EmployeeID = " . $_POST['EmployeeID'];

                if ($conn->query($sql) === TRUE) {
                    echo "Record was updated successfully <br />";
                    echo "<a href = 'list_dotask.php'>Go back</a>";
                } else {
                    echo "Error updating record: " . $conn->error;
                }
            }  
        }
        $conn->close();
    ?>

</body>
</html>
