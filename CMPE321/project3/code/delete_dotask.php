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

            // Fetch the record
            $sql = "SELECT TaskID, EmployeeID FROM dotask WHERE TaskID = " . $_GET['taskid'] . " AND EmployeeID = " . $_GET['employeeid'];
            $result = $conn->query($sql);

            // If the record actually exists
            if ($result->num_rows > 0) {
                ?>
                <form action="delete_dotask_result.php" method="post">
                <?php

                // Get the data
                $row = $result->fetch_assoc();
                ?>
                    Are you sure you want to delete the following record? <br />
                    <p>Task ID: <input type="text" name="TaskID" value = "<?php echo $row["TaskID"] ?>" readonly /></p>
                    <p>Employee ID: <input type="text" name="EmployeeID" value = "<?php echo $row["EmployeeID"] ?>" readonly /></p>
                    <p><input type="submit" value = "Delete Record" /></p>
                </form>
                <?php
            } else {
                echo "Record does not exist";
            }
        }
        $conn->close();
    ?>

</body>
</html>
