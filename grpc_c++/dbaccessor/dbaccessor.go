package main

import (
	"context"
	"database/sql"
	_ "github.com/go-sql-driver/mysql"
	"google.golang.org/grpc"
	"log"
	"net"

	pb "Jeon_2020/5.grpc_chat/protos/gen/go"
)

type DbaccessorServer struct {
	pb.UnimplementedDbaccessorServer

	db *sql.DB
}

func (s *DbaccessorServer) InsertMessage(ctx context.Context, message *pb.Message) (*pb.SimpleResponse, error) {
	_, err := s.db.Exec("INSERT INTO message (user, msg, time) VALUES (?, ?, now(6))", message.UserId, message.Text)
	if err != nil {
    log.Fatal(err)
	}

	return &pb.SimpleResponse{ResponseFlag: true}, nil
}

func newDbaccessorServer() *DbaccessorServer {
	DB, err := sql.Open("mysql", "root:123123@tcp(127.0.0.1:3306)/chat")
	if err != nil {
		log.Fatal(err)
	}
	s := &DbaccessorServer{db: DB}

	return s
}

func main() {
	lis, err := net.Listen("tcp", "localhost:50052")
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}

	gRPCServer := grpc.NewServer()
	pb.RegisterDbaccessorServer(gRPCServer, newDbaccessorServer())

	gRPCServer.Serve(lis)
}
