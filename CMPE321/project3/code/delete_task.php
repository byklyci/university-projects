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
            $sql = "SELECT ID, ProjectID, StartDate, DayItTakes FROM task WHERE ID = " . $_GET['id'] . " AND ProjectID = " . $_GET['projectid'];
            $result = $conn->query($sql);

            // If the record actually exists
            if ($result->num_rows > 0) {
                ?>
                <form action="delete_task_result.php" method="post">
                <?php

                // Get the data
                $row = $result->fetch_assoc();
                ?>
                    Are you sure you want to delete the following record? <br />
                    <p>ID: <input type="text" name="ID" value = "<?php echo $row["ID"] ?>" readonly /></p>
                    <p>Project ID: <input type="text" name="ProjectID" value = "<?php echo $row["ProjectID"] ?>" readonly /></p>
                    <p>Start Date: <input type="text" name="StartDate" value = "<?php echo $row["StartDate"] ?>" readonly /></p>
                    <p>Day It Takes: <input type="text" name="DayItTakes" value = "<?php echo $row["DayItTakes"] ?>" readonly /></p>
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
