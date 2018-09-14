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

                session_start();
                $sql2 = "SELECT ProjectID FROM manageproject WHERE ProjectManagerID = " . $_SESSION['ID'];
                $result2 = $conn->query($sql2);

                if ($result2->num_rows > 0) {

                    ?>
                    <form action="edit_task_result.php" method="post">
                    <?php

                    // Get the data
                    $row = $result->fetch_assoc();
                    ?>
                        <p>ID: <input type="text" name="ID" value = "<?php echo $row["ID"] ?>" readonly /></p>
                        <p>Project ID: 
                        <?php 
                            $select= '<select name="NewProjectID">';
                            while($row2 = $result2->fetch_assoc()){
                                if($row2['ProjectID']==$row["ProjectID"]){
                                    $select.='<option value="'.$row2['ProjectID'].'" selected>'.$row2['ProjectID'].'</option>';
                                }else{
                                    $select.='<option value="'.$row2['ProjectID'].'">'.$row2['ProjectID'].'</option>';
                                }
                                
                            }
                            $select.='</select>';
                            echo $select;
                        ?>
                        </p>
                        <p>Start Date: <input type="text" name="StartDate" value = "<?php echo $row["StartDate"] ?>" /></p>
                        <p>Day It Takes: <input type="text" name="DayItTakes" value = "<?php echo $row["DayItTakes"] ?>" /></p>
                        <input type="hidden" name="ProjectID" value="<?php echo $_GET['projectid'] ?>">
                        <p><input type="submit" value = "Save Changes" /></p>
                    </form>

                    <?php
                } else {
                    echo "You do not have any projects assigned to you.";
                }
            } else {
                echo "Record does not exist";
            }
        }
        $conn->close();
    ?>

</body>
</html>
