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
            $sql = "SELECT ProjectID, ProjectManagerID FROM manageproject WHERE ProjectID = " . $_GET['projectid'] . " AND ProjectManagerID = " . $_GET['projectmanagerid'];
            $result = $conn->query($sql);

            // If the record actually exists
            if ($result->num_rows > 0) {
                ?>
                <form action="edit_manageproject_result.php" method="post">

                <?php

                // Get the data
                $row = $result->fetch_assoc();
                ?>
                    <p>Project ID: <input type="text" name="NewProjectID" value = "<?php echo $row["ProjectID"] ?>" /></p>
                    <p>Project Manager ID: <input type="text" name="NewProjectManagerID" value = "<?php echo $row["ProjectManagerID"] ?>" /></p>
                    <input type="hidden" name="ProjectID" value="<?php echo $_GET['projectid'] ?>">
                    <input type="hidden" name="ProjectManagerID" value="<?php echo $_GET['projectmanagerid'] ?>">
                    <p><input type="submit" value = "Save Changes" /></p>
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
